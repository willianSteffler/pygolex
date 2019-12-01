package main

import (
	"fmt"
	"gitlab.com/wsteffler/pygolex/lex"
	"os"
)

func main(){
	args := os.Args[1:]

	if len(args) == 2 {
		fmt.Println("Iniciando análise léxica ...")
		py := lex.PyLex{}
		tokens,err := py.Analyse(args[0])

		if err != nil {
			fmt.Printf("Erro! na linha %d coluna %d : %v \n",py. Cursor.Row,py. Cursor.Column,err)
		} else {
			fmt.Println("Lista de Tokens:")
			for _, t := range tokens {
				fmt.Println(fmt.Sprintf("Token (Type: %d, Value: '%s', Line: %d, Column: %d)",t.Type,t.Value,t.Row,t.Column))
			}

			fmt.Println("Iniciando análise da gramática ...")
			gram := lex.Pygram{}
			gram.Init(tokens)
			err = gram.Analyze()
			if err != nil {
				fmt.Printf("Erro! token %+v : %v \n",gram.Tk,err)
			} else {

				f, err := os.Create(args[1])
				if err != nil {
					fmt.Println(err)
					f.Close()
					return
				}

				for _, t := range tokens {
					_,err = fmt.Fprintln(f, fmt.Sprintf("Token (Type: %d, Value: '%s', Line: %d, Column: %d)",t.Type,t.Value,t.Row,t.Column))
					if err != nil {
						fmt.Println(err)
						return
					}
				}
				err = f.Close()
				if err != nil {
					fmt.Println(err)
					return
				}
				fmt.Println("saida gerada para o arquivo %s",args[1])
			}
		}
	} else {
		fmt.Println("Erro! número de argumentos inválido")
	}
}