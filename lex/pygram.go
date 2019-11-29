package lex

import (
	"errors"
	"fmt"
	"gitlab.com/wsteffler/pygolex/lextoken"
)

type Pygram struct {
	Tokens   []lextoken.LexToken
	pos      int
	tk       lextoken.LexToken
	funcStmt bool
}

func(pg *Pygram) Next(){
	pg.tk = pg.Tokens[pg.pos]
	pg.pos++
}

func(pg *Pygram) Init(tks []lextoken.LexToken){
	pg.Tokens = tks
}

func(pg *Pygram) Stmt() (err error) {
	if pg.tk.Type==lextoken.TK_IF {
		return pg.IfStmt()
	} else if pg.tk.Type==lextoken.TK_IDENTIFIER {
		return ExpStmt()
	} else if pg.tk.Type==lextoken.TK_WHILE {
		return WhileStmt()
	} else if pg.tk.Type==lextoken.TK_FOR {
		return ForStmt()
	} else if pg.tk.Type==lextoken.TK_RETURN && pg.funcStmt {
		return RetStmt()
	} else if pg.tk.Type==lextoken.TK_DEF && !pg.funcStmt {
		pg.funcStmt = true
		err = DefStmt()
		pg.funcStmt = false
		return err
	}

	return err
}

func(pg *Pygram) IfStmt() (err error) {
	pg.Next()
	if err = Rel(); err == nil{
		if pg.tk.Type== lextoken.TK_TWO_DOTS {
			pg.Next()
			if pg.tk.Type== lextoken.TK_INDENT {
				pg.Next()
				if err = CompStmt(); err == nil {
					if pg.tk.Type== lextoken.TK_ELSE {
						pg.Next()
						if pg.tk.Type== lextoken.TK_TWO_DOTS {
							pg.Next()
							if pg.tk.Type== lextoken.TK_INDENT {
								pg.Next()
								if err = CompStmt(); err == nil {
									return nil
								} else {
									return errors.New(fmt.Sprintf("erro no comando do else : %v",err))
								}
							} else {
								return errors.New("identação incorreta")
							}
						} else {
							return errors.New("espera dois pontos")
						}
					} else {
						return nil
					}
				} else {
					return errors.New(fmt.Sprintf("erro no bloco do if %v",err))
				}
			} else {
				return errors.New("identação incorreta")
			}

		} else {
			return errors.New("esperava dois pontos")
		}
	} else {
		return errors.New(fmt.Sprintf("erro na expressão do if : %v",err))
	}
}