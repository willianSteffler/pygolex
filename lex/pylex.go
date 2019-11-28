package lex

import (
	"errors"
	"gitlab.com/wsteffler/pygolex/lex/cursor"
	"gitlab.com/wsteffler/pygolex/lextoken"
	"log"
	"os"
	"runtime/debug"
	"strings"
)

type State int
const (
	initialState State = iota + 1
	stringState
	commentState
	operatorDelimiterState
	numberState
	keywordState
)

type PyLex struct {
	Tokens []lextoken.LexToken
	indents Stack
	cursor cursor.Cursor
	state State
 }

type Stack struct {
	s []int
}

func(s *Stack)Top() int{
	return s.s[len(s.s) - 1]
}

func(s *Stack)Push(n int ){
	s.s = append(s.s, n)
}

func(s * Stack)Pop(){
	s.s = s.s[:(len(s.s) - 1)]
}

func(s * Stack)Len() int{
	return len(s.s)
}



func (py *PyLex) Analyse(filename string) ([]lextoken.LexToken,error){
	f,err := os.Open(filename)
	defer f.Close()
	defer func() {
		if r := recover(); r != nil {
			log.Fatal(r,string(debug.Stack()))
		}
	}()

	if err != nil{
		log.Fatal(err)
	}

	py.cursor = cursor.Cursor{}
	py.cursor.Init(f)
	py.state = initialState

	ok := true
	for err == nil && ok {
		if err,ok = py.scanIndentation(); ok{
			err,ok = py.getToken()
			for err == nil && py.cursor.LastChar != '\n' && ok{
				err,ok = py.getToken()
			}

			if !ok {
				break
			}
		}
	}

	return py.Tokens,err
}

func(py *PyLex) scanIndentation() (error,bool) {
	n := 0
	c,ok := py.cursor.Next()
	for c == ' ' && ok {
		c,ok = py.cursor.Next()
		n++
	}

	//py.cursor.Back()

	if n > 0 && py.indents.Len() == 0 || py.indents.Len() > 0  && n>= py.indents.Top()  {
		py.indents.Push(n)
		py.Tokens = append (py.Tokens, lextoken.LexToken{
			Row:    py.cursor.Row,
			Column: py.cursor.Column,
			Type:   lextoken.TK_INDENT,
			Value:  "INDENT",
		})
	} else if py.indents.Len() != 0 {
	py.indents.Pop()
		py.Tokens = append (py.Tokens, lextoken.LexToken{
			Row:    py.cursor.Row,
			Column: py.cursor.Column,
			Type:   lextoken.TK_DEINDENT,
			Value:  "DEINDENT",
		})
	}

	return nil,ok
}

func(py *PyLex) getToken() (error,bool) {
	var lex string
	hasNextChar := true

	switch py.state {
	case initialState:
		if py.cursor.LastChar == ' '{
			_,hasNextChar = py.cursor.Next()
			return nil,hasNextChar
		}
		if (py.cursor.LastChar >= '0' && py.cursor.LastChar <= '9') || py.cursor.LastChar == '-' || py.cursor.LastChar == '.'  {
			py.state = numberState
			return py.getToken()
		}

		if (py.cursor.LastChar >= 'a' && py.cursor.LastChar <= 'z') || (py.cursor.LastChar >= 'A' && py.cursor.LastChar <= 'Z') || py.cursor.LastChar == '_' {
			py.state = keywordState
			return py.getToken()

		}

		if py.cursor.LastChar == '"' || py.cursor.LastChar == '\'' {
			py.state = stringState
			return py.getToken()
		}

		if py.cursor.LastChar == '#' {
			py.state = commentState
			return py.getToken()

		}

		py.state = operatorDelimiterState
		return py.getToken()

	case numberState:
		for ((py.cursor.LastChar >= '0' && py.cursor.LastChar <= '9') ||
			py.cursor.LastChar == '.' ||
			py.cursor.LastChar == 'j' ||
			py.cursor.LastChar == 'e' ||
			py.cursor.LastChar == 'E'||
			py.cursor.LastChar == '-') && hasNextChar{
			if (strings.ContainsRune(lex,'j') && py.cursor.LastChar == 'j') ||
				(strings.ContainsRune(lex,'.') && py.cursor.LastChar == '.')||
				(strings.ContainsRune(lex,'e') && py.cursor.LastChar == 'e')||
				(strings.ContainsRune(lex,'E') && py.cursor.LastChar == 'E')||
				(strings.ContainsRune(lex,'-') && py.cursor.LastChar == '-'){
				return errors.New("caractere nao esperado"),hasNextChar
			}

			lex += string(py.cursor.LastChar)
			_,hasNextChar = py.cursor.Next()
		}

		val := strings.ToLower(lex)
		if s := strings.Split(val,"e"); len(s) > 1 && (s[1][0] <= '0' || s[1][0] >= '9') {
			return errors.New("valor numerico inesperado"),hasNextChar
		}

		t := lextoken.LexToken{
			Row:    py.cursor.Row,
			Column: py.cursor.Column,
			Type:   lextoken.TK_NUMBER,
			Value:  lex,
		}

		py.Tokens = append (py.Tokens, t)
		py.state = initialState
	break

	case keywordState:
		for ((py.cursor.LastChar >= 'a' && py.cursor.LastChar <= 'z') ||
			(py.cursor.LastChar >= 'A' && py.cursor.LastChar <= 'Z') ||
			py.cursor.LastChar == '_' ||
			py.cursor.LastChar == '.' ||
			(py.cursor.LastChar >= '0' && py.cursor.LastChar <= '9')) && hasNextChar {
			lex += string(py.cursor.LastChar)
			_,hasNextChar = py.cursor.Next()
		}

		tk,ok := lextoken.Keywords[lex]
		if !ok {
			tk = lextoken.TK_IDENTIFIER
		}

		t := lextoken.LexToken{
			Row:    py.cursor.Row,
			Column: py.cursor.Column,
			Type:   tk,
			Value:  lex,
		}

		py.Tokens = append (py.Tokens, t)
		py.state = initialState

	break

	case stringState:
		_,hasNextChar = py.cursor.Next()
		if !hasNextChar {
			return errors.New("final de arquivo inesperado"),hasNextChar
		}

		for py.cursor.LastChar != '"' && py.cursor.LastChar != '\'' && hasNextChar {
			lex += string(py.cursor.LastChar)
			_,hasNextChar = py.cursor.Next()
			if !hasNextChar || py.cursor.LastChar == '\n' {
				return errors.New("não encontrado final de string \" ou '"),hasNextChar
			}
		}

		_,hasNextChar = py.cursor.Next()

		t := lextoken.LexToken{
			Row:    py.cursor.Row,
			Column: py.cursor.Column,
			Type:   lextoken.TK_STRING,
			Value:  lex,
		}

		py.Tokens = append (py.Tokens, t)
		py.state = initialState

	break

	case commentState:
		_,hasNextChar := py.cursor.Next()

		for py.cursor.LastChar != '\n' && hasNextChar {
			lex += string(py.cursor.LastChar)
			_,hasNextChar = py.cursor.Next()
		}

		t := lextoken.LexToken{
			Row:    py.cursor.Row,
			Column: py.cursor.Column,
			Type:   lextoken.TK_COMMENT,
			Value:  lex,
		}

		py.Tokens = append (py.Tokens, t)
		py.state = initialState

	break

	case operatorDelimiterState:
		var tk lextoken.TokenT
		lex += string(py.cursor.LastChar)
		if py.cursor.LastChar != '\n' {

			for isOpDelim(lex) && hasNextChar{
				_,hasNextChar = py.cursor.Next()
				lex += string(py.cursor.LastChar)
			}

			ok := false
			lex = lex[:len(lex)-1]
			tk,ok = operatorDelimiter(lex)

			if ! ok {
				if !hasNextChar {
					return errors.New("final de arquivo inesperado"),hasNextChar
				}
				return errors.New("caractere não esperado"),hasNextChar
			}
		} else {
			tk = lextoken.TK_NEWLINE
		}

		t := lextoken.LexToken{
			Row:    py.cursor.Row,
			Column: py.cursor.Column,
			Type:   tk,
			Value:  lex,
		}

		py.Tokens = append (py.Tokens, t)
		py.state = initialState
	break

	default:
		break
	}

	return nil,hasNextChar
}

func operatorDelimiter( v string ) (lextoken.TokenT,bool){
	tk,ok := lextoken.Operators[v]
	if !ok{
		tk,ok = lextoken.Delimiters[v]
	}

	return  tk,ok
}

func isOpDelim(v string)bool{
	for key, _ := range lextoken.Operators {
		if strings.Contains(key,v){
			return true
		}
	}

	for key, _ := range lextoken.Delimiters {
		if strings.Contains(key,v){
			return true
		}
	}

	return false
}