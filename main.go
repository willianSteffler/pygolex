package main

import (
	"fmt"
	"gitlab.com/wsteffler/pygolex/lex"
	"os"
)

func main(){
	args := os.Args[1:]

	if len(args) == 1 {
		py := lex.PyLex{}
		tokens,err := py.Analyse(args[0])

		if err != nil {
			fmt.Println("Erro! %s ",err)
		} else {
			for _, t := range tokens {
				fmt.Println(fmt.Sprintf("Token (Type: %d, Value: '%s', Line: %d, Column: %d)",t.Type,t.Value,t.Row,t.Column))
			}
		}
	} else {
		fmt.Println("Erro! número de argumentos inválido")
	}
}