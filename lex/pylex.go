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
	Tokens  []lextoken.LexToken
	indents Stack
	Cursor  cursor.Cursor
	state   State
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

	py.Cursor = cursor.Cursor{}
	py.Cursor.Init(f)
	py.state = initialState

	ok := true
	for err == nil && ok {
		if err,ok = py.scanIndentation(); ok{
			err,ok = py.getToken()
			for err == nil && py.Cursor.LastChar != '\n' && ok{
				err,ok = py.getToken()
			}

			if !ok {
				break
			}else {
				t := lextoken.LexToken{
					Row:    py.Cursor.Row,
					Column: py.Cursor.Column,
					Type:   lextoken.TK_NEWLINE,
					Value:  "NEWLINE",
				}

				py.Tokens = append(py.Tokens,t)
			}
		}
	}

	if err == nil {
		t := lextoken.LexToken{
			Row:    py.Cursor.Row,
			Column: py.Cursor.Column,
			Type:   lextoken.TK_EOF,
			Value:  "EOT",
		}
		py.Tokens = append(py.Tokens,t)
	}

	return py.Tokens,err
}

func(py *PyLex) scanIndentation() (error,bool) {
	n := 0
	c,ok := py.Cursor.Next()
	for c == ' ' && ok {
		c,ok = py.Cursor.Next()
		n++
	}

	//py.Cursor.Back()

	if n > 0 && py.indents.Len() == 0 || py.indents.Len() > 0  && n>= py.indents.Top()  {
		py.indents.Push(n)
		py.Tokens = append (py.Tokens, lextoken.LexToken{
			Row:    py.Cursor.Row,
			Column: py.Cursor.Column,
			Type:   lextoken.TK_INDENT,
			Value:  "INDENT",
		})
	} else if py.indents.Len() != 0 {
	py.indents.Pop()
		py.Tokens = append (py.Tokens, lextoken.LexToken{
			Row:    py.Cursor.Row,
			Column: py.Cursor.Column,
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
		if py.Cursor.LastChar == ' '{
			_,hasNextChar = py.Cursor.Next()
			return nil,hasNextChar
		}
		if (py.Cursor.LastChar >= '0' && py.Cursor.LastChar <= '9') || py.Cursor.LastChar == '-' || py.Cursor.LastChar == '.'  {
			py.state = numberState
			return py.getToken()
		}

		if (py.Cursor.LastChar >= 'a' && py.Cursor.LastChar <= 'z') || (py.Cursor.LastChar >= 'A' && py.Cursor.LastChar <= 'Z') || py.Cursor.LastChar == '_' {
			py.state = keywordState
			return py.getToken()

		}

		if py.Cursor.LastChar == '"' || py.Cursor.LastChar == '\'' {
			py.state = stringState
			return py.getToken()
		}

		if py.Cursor.LastChar == '#' {
			py.state = commentState
			return py.getToken()

		}

		py.state = operatorDelimiterState
		return py.getToken()

	case numberState:
		for ((py.Cursor.LastChar >= '0' && py.Cursor.LastChar <= '9') ||
			py.Cursor.LastChar == '.' ||
			py.Cursor.LastChar == 'j' ||
			py.Cursor.LastChar == 'e' ||
			py.Cursor.LastChar == 'E'||
			py.Cursor.LastChar == '-') && hasNextChar{
			if (strings.ContainsRune(lex,'j') && py.Cursor.LastChar == 'j') ||
				(strings.ContainsRune(lex,'.') && py.Cursor.LastChar == '.')||
				(strings.ContainsRune(lex,'e') && py.Cursor.LastChar == 'e')||
				(strings.ContainsRune(lex,'E') && py.Cursor.LastChar == 'E')||
				(strings.ContainsRune(lex,'-') && py.Cursor.LastChar == '-'){
				return errors.New("caractere nao esperado"),hasNextChar
			}

			lex += string(py.Cursor.LastChar)
			_,hasNextChar = py.Cursor.Next()
		}

		val := strings.ToLower(lex)
		if s := strings.Split(val,"e"); len(s) > 1 && (s[1][0] <= '0' || s[1][0] >= '9') {
			return errors.New("valor numerico inesperado"),hasNextChar
		}

		tktype := lextoken.TK_INTEGER
		if strings.Contains(val,"e") ||  strings.Contains(val,"."){
			tktype = lextoken.TK_FLOAT
		} else if strings.Contains(val,"j"){
			tktype = lextoken.TK_COMPLEX
		}

		t := lextoken.LexToken{
			Row:    py.Cursor.Row,
			Column: py.Cursor.Column,
			Type:   tktype,
			Value:  lex,
		}

		py.Tokens = append (py.Tokens, t)
		py.state = initialState
	break

	case keywordState:
		for ((py.Cursor.LastChar >= 'a' && py.Cursor.LastChar <= 'z') ||
			(py.Cursor.LastChar >= 'A' && py.Cursor.LastChar <= 'Z') ||
			py.Cursor.LastChar == '_' ||
			py.Cursor.LastChar == '.' ||
			(py.Cursor.LastChar >= '0' && py.Cursor.LastChar <= '9')) && hasNextChar {
			lex += string(py.Cursor.LastChar)
			_,hasNextChar = py.Cursor.Next()
		}

		tk,ok := lextoken.Keywords[lex]
		if !ok {
			tk = lextoken.TK_IDENTIFIER
		}

		t := lextoken.LexToken{
			Row:    py.Cursor.Row,
			Column: py.Cursor.Column,
			Type:   tk,
			Value:  lex,
		}

		py.Tokens = append (py.Tokens, t)
		py.state = initialState

	break

	case stringState:
		_,hasNextChar = py.Cursor.Next()
		if !hasNextChar {
			return errors.New("final de arquivo inesperado"),hasNextChar
		}

		for py.Cursor.LastChar != '"' && py.Cursor.LastChar != '\'' && hasNextChar {
			lex += string(py.Cursor.LastChar)
			_,hasNextChar = py.Cursor.Next()
			if !hasNextChar || py.Cursor.LastChar == '\n' {
				return errors.New("não encontrado final de string \" ou '"),hasNextChar
			}
		}

		_,hasNextChar = py.Cursor.Next()

		t := lextoken.LexToken{
			Row:    py.Cursor.Row,
			Column: py.Cursor.Column,
			Type:   lextoken.TK_STRING,
			Value:  lex,
		}

		py.Tokens = append (py.Tokens, t)
		py.state = initialState

	break

	case commentState:
		_,hasNextChar := py.Cursor.Next()

		for py.Cursor.LastChar != '\n' && hasNextChar {
			lex += string(py.Cursor.LastChar)
			_,hasNextChar = py.Cursor.Next()
		}

		t := lextoken.LexToken{
			Row:    py.Cursor.Row,
			Column: py.Cursor.Column,
			Type:   lextoken.TK_COMMENT,
			Value:  lex,
		}

		py.Tokens = append (py.Tokens, t)
		py.state = initialState

	break

	case operatorDelimiterState:
		var tk lextoken.TokenT
		lex += string(py.Cursor.LastChar)
		if py.Cursor.LastChar != '\n' {

			for isOpDelim(lex) && hasNextChar{
				_,hasNextChar = py.Cursor.Next()
				lex += string(py.Cursor.LastChar)
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
			py.state = initialState
			return nil,hasNextChar
		}

		t := lextoken.LexToken{
			Row:    py.Cursor.Row,
			Column: py.Cursor.Column,
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