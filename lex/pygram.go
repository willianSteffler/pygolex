package lex

import (
	"errors"
	"fmt"
	"gitlab.com/wsteffler/pygolex/lextoken"
)

type Pygram struct {
	Tokens   []lextoken.LexToken
	pos      int
	Tk       lextoken.LexToken
	funcStmt bool
}

func (pg *Pygram) Next() {
	pg.Tk = pg.Tokens[pg.pos]
	pg.pos++
}

func (pg *Pygram) Init(tks []lextoken.LexToken) {
	pg.Tokens = tks
}

func (pg *Pygram) Analyze() (err error) {
	pg.Next()
	for pg.Tk.Type != lextoken.TK_EOF && err == nil{
		err = pg.Stmt()
	}

	return err
}

func (pg *Pygram) Stmt() (err error) {
	if pg.Tk.Type == lextoken.TK_NEWLINE ||  pg.Tk.Type == lextoken.TK_INDENT || pg.Tk.Type == lextoken.TK_DEINDENT{
		pg.Next()
		return nil
	} else if pg.Tk.Type == lextoken.TK_IF {
		return pg.IfStmt()
	} else if pg.Tk.Type == lextoken.TK_IDENTIFIER {
		return pg.ExpStmt()
	} else if pg.Tk.Type == lextoken.TK_WHILE {
		return pg.WhileStmt()
	} else if pg.Tk.Type == lextoken.TK_FOR {
		return pg.ForStmt()
	} else if pg.Tk.Type == lextoken.TK_RETURN && pg.funcStmt {
		return pg.RetStmt()
	} else if pg.Tk.Type == lextoken.TK_DEF && !pg.funcStmt {
		pg.funcStmt = true
		err = pg.DefStmt()
		pg.funcStmt = false
		return err
	}

	return err
}

func (pg *Pygram) RangeFunc() (err error) {
	if pg.Tk.Type == lextoken.TK_RANGE || pg.Tk.Type == lextoken.TK_XRANGE {
		pg.Next()
		if pg.Tk.Type == lextoken.TK_OPEN_PARENTESI {
			pg.Next()
			if pg.Tk.Type == lextoken.TK_INTEGER {
				pg.Next()
				if pg.Tk.Type == lextoken.TK_COMA {
					pg.Next()
					if pg.Tk.Type == lextoken.TK_INTEGER {
						pg.Next()
						if pg.Tk.Type == lextoken.TK_COMA {
							pg.Next()
							if pg.Tk.Type == lextoken.TK_INTEGER {
								pg.Next()
							} else {
								err = errors.New("esperava um inteiro")
							}
						}
					} else {
						err = errors.New("esperava um inteiro")
					}
				}
			} else {
				err = errors.New("esperava um inteiro")
			}
			if err == nil {
				if pg.Tk.Type != lextoken.TK_CLOSE_PARENTESI {
					err = errors.New("esperava fechar parenteses")
				}
			}
		} else {
			err = errors.New("esperava abrir parenteses")
		}
	} else {
		err = errors.New("esperava token range ou xrange")
	}

	return err
}

func (pg *Pygram) ForStmt() (err error) {
	pg.Next()
	if pg.Tk.Type == lextoken.TK_IDENTIFIER {
		pg.Next()
		if pg.Tk.Type == lextoken.TK_IN {
			pg.Next()
			if pg.Tk.Type != lextoken.TK_IDENTIFIER {
				err = pg.RangeFunc()
			}

			if err == nil {
				pg.Next()
				if pg.Tk.Type == lextoken.TK_TWO_DOTS {
					pg.Next()
					if pg.Tk.Type == lextoken.TK_NEWLINE {
						pg.Next()
						if pg.Tk.Type == lextoken.TK_INDENT {
							pg.Next()
							err = pg.CompStmt()
						} else {
							err = errors.New("esperava identação para o bloco do FOR")
						}
					} else {
						err = errors.New("esperava nova linha para o bloco do FOR")
					}
				} else {
					err = errors.New("esperava dois pontos")
				}
			}
		} else {
			err = errors.New("esperava a palavra reservada IN")
		}
	} else {
		err = errors.New("esperava um ID")
	}

	return err
}

func (pg *Pygram) RetStmt() (err error) {
	pg.Next()
	err = pg.A()
	return err
}

func (pg *Pygram) WhileStmt() (err error) {
	pg.Next()
	if err = pg.Rel(); err == nil {
		if pg.Tk.Type == lextoken.TK_TWO_DOTS {
			pg.Next()
			if pg.Tk.Type == lextoken.TK_NEWLINE {
				pg.Next()
				if pg.Tk.Type == lextoken.TK_INDENT {
					pg.Next()
					err = pg.CompStmt()
				} else {
					err = errors.New("identação incorreta")
				}
			} else {
				err = errors.New("esperava nova linha")
			}

		} else {
			err = errors.New("esperava dois pontos")
		}
	}
	return err
}

func (pg *Pygram) DefStmt() (err error) {
	pg.Next()
	if pg.Tk.Type != lextoken.TK_IDENTIFIER {
		return errors.New("esperava o ID da função a ser declarada")
	}
	pg.Next()
	if pg.Tk.Type != lextoken.TK_OPEN_PARENTESI {
		return errors.New("esperava abrir parenteses")
	}
	pg.Next()
	for pg.Tk.Type == lextoken.TK_IDENTIFIER {
		pg.Next()
		if pg.Tk.Type != lextoken.TK_CLOSE_PARENTESI && pg.Tk.Type != lextoken.TK_COMA {
			return errors.New("esperava fechar parenteses ou virgula")
		}
		if pg.Tk.Type == lextoken.TK_COMA{
			pg.Next()
		} 
	}

	if pg.Tk.Type != lextoken.TK_CLOSE_PARENTESI {
		return errors.New("esperava fechar parenteses")
	}
	pg.Next()
	if pg.Tk.Type != lextoken.TK_TWO_DOTS {
		return errors.New("esperava dois pontos na declaração da função")
	}
	if pg.Tk.Type != lextoken.TK_NEWLINE{
		return errors.New("esperava nova linha")
	}

	pg.Next()
	if pg.Tk.Type != lextoken.TK_INDENT {
		return errors.New("erro na identação da função")
	}
	pg.Next()
	err = pg.CompStmt()
	return err
}

func (pg *Pygram) IfStmt() (err error) {
	pg.Next()
	if err = pg.Rel(); err == nil {
		if pg.Tk.Type == lextoken.TK_TWO_DOTS {
			pg.Next()
			if pg.Tk.Type == lextoken.TK_NEWLINE {
				pg.Next()
				if pg.Tk.Type == lextoken.TK_INDENT {
					pg.Next()
					if err = pg.CompStmt(); err == nil {
						if pg.Tk.Type == lextoken.TK_ELSE {
							pg.Next()
							if pg.Tk.Type == lextoken.TK_TWO_DOTS {
								pg.Next()
								if pg.Tk.Type == lextoken.TK_NEWLINE {
									pg.Next()
									if pg.Tk.Type == lextoken.TK_INDENT {
										pg.Next()
										if err = pg.CompStmt(); err == nil {
											return nil
										} else {
											return errors.New(fmt.Sprintf("erro no comando do else : %v", err))
										}
									} else {
										return errors.New("identação incorreta")
									}
								} else {
									return errors.New("esperava nova linha")
								}

							} else {
								return errors.New("espera dois pontos")
							}
						} else {
							return nil
						}
					} else {
						return errors.New(fmt.Sprintf("erro no bloco do if %v", err))
					}
				} else {
					return errors.New("identação incorreta")
				}
			} else {
				return errors.New("esperava nova linha")
			}

		} else {
			return errors.New("esperava dois pontos")
		}
	} else {
		return errors.New(fmt.Sprintf("erro na expressão do if : %v", err))
	}
}

func (pg *Pygram) A() (err error) {
	if err = pg.Rel(); err == nil {
		if pg.Tk.Type != lextoken.TK_ATTR {
			return nil
		}
		pg.Next()
		err = pg.A()
	}
	return err
}

func (pg *Pygram) Rel() (err error) {
	if err = pg.E(); err == nil {
		if pg.Tk.Type == lextoken.TK_GREATER ||
			pg.Tk.Type == lextoken.TK_LOWER ||
			pg.Tk.Type == lextoken.TK_IGUAL ||
			pg.Tk.Type == lextoken.TK_DIFF ||
			pg.Tk.Type == lextoken.TK_LOWER_EQUAL ||
			pg.Tk.Type == lextoken.TK_GREATER_EQUAL {

			pg.Next()
			err = pg.E()
		}
	}
	return err
}

func (pg *Pygram) E() (err error) {
	if err = pg.T(); err == nil {
		err = pg.R()
	}
	return err
}

func (pg *Pygram) R() (err error) {
	if pg.Tk.Type == lextoken.TK_SUM || pg.Tk.Type == lextoken.TK_SUB {
		pg.Next()
		if err = pg.T(); err == nil {
			err = pg.R()
		}
	}
	return err
}
func (pg *Pygram) T() (err error) {
	if err = pg.F(); err == nil {
		err = pg.S()
	}
	return err
}

func (pg *Pygram) S() (err error) {
	if pg.Tk.Type == lextoken.TK_MUL || pg.Tk.Type == lextoken.TK_DIV {
		pg.Next()

		if err = pg.F(); err == nil {
			err = pg.S()
		}
	}
	return err
}

func (pg *Pygram) F() (err error) {
	if pg.Tk.Type == lextoken.TK_STRING ||
		pg.Tk.Type == lextoken.TK_INTEGER ||
		pg.Tk.Type == lextoken.TK_FLOAT ||
		pg.Tk.Type == lextoken.TK_COMPLEX ||
		pg.Tk.Type == lextoken.TK_TRUE ||
		pg.Tk.Type == lextoken.TK_FALSE {
		pg.Next()
	} else if pg.Tk.Type == lextoken.TK_IDENTIFIER {
		pg.Next()
		if pg.Tk.Type == lextoken.TK_OPEN_PARENTESI {
			err = pg.CallFuncStmt()
		}
	} else if pg.Tk.Type == lextoken.TK_OPEN_PARENTESI {
		pg.Next()
		if err = pg.E(); err == nil {
			if pg.Tk.Type == lextoken.TK_CLOSE_PARENTESI {
				pg.Next()
			} else {
				err = errors.New("esperava fecha parênteses")
			}
		}
	} else {
		err = errors.New("token invalido")
	}

	return err
}

func (pg *Pygram) CallFuncStmt() (err error) {
	pg.Next()
	for pg.Tk.Type != lextoken.TK_CLOSE_PARENTESI {
		if pg.Rel(); err != nil {
			return err
		}

		if pg.Tk.Type != lextoken.TK_COMA && pg.Tk.Type != lextoken.TK_CLOSE_PARENTESI {
			return errors.New("esperava virgula ou fecha parenteses")
		}
		if pg.Tk.Type == lextoken.TK_COMA {
			pg.Next()
		}
	}

	if pg.Tk.Type == lextoken.TK_CLOSE_PARENTESI {
		pg.Next()
	} else {
		err = errors.New("esperava fecha parenteses")
	}

	return err
}

func (pg *Pygram) CompStmt() (err error) {
	for pg.Tk.Type != lextoken.TK_DEINDENT && pg.Tk.Type != lextoken.TK_EOF {
		if err = pg.Stmt(); err != nil {
			return err
		}
	}

	if pg.Tk.Type == lextoken.TK_DEINDENT {
		pg.Next()
	}
	return nil
}

func (pg *Pygram) ExpStmt() (err error) {
	if err = pg.A(); err == nil {
		if pg.Tk.Type == lextoken.TK_NEWLINE {
			return nil
		} else {
			return errors.New("esperava nova linha")
		}
	}

	return err
}
