#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// DEFINE TOKENS

#define TK1 1 // ;
#define TK2 2 // ...
#define TK3 3 // (Test)
#define TK4 4 // (Star_Expr)
#define TK5 5 // (,
#define TK6 6 // (Test))
#define TK7 7 // (Star_Expr))
#define TK8 8 // ,
#define TK9 9 // +
#define TK10 10 // id
#define TK11 11 // (Exp)
#define TK12 12 // :=
#define TK13 13 // if
#define TK14 14 // else
#define TK15 15 // or
#define TK16 16 // and
#define TK17 17 // not
#define TK18 18 // '<'
#define TK19 19 // '>'
#define TK20 20 // '=='
#define TK21 21 // '>='
#define TK22 22 // '<='
#define TK23 23 // '<>'
#define TK24 24 // '!='
#define TK25 25 // 'in'
#define TK26 26 // 'not'
#define TK27 27 // 'is'
#define TK28 28 // :
#define TK29 29 // elif
#define TK30 30 // ident
#define TK31 31 // dedent
#define TK32 32 // for
#define TK33 33 // in
#define TK34 34 // id:
#define TK35 35 // while
#define TK36 36 // def
#define TK37 37 // nome
#define TK38 38 // (List_Params)

// Variáveis Globais <o>

int tk;
int tklinha = 0;
int tkcoluna = 0;

// <*********** INICIO DO ANALISADOR LÉXICO ***********>

// Implemente aqui seu analisador Léxico

// <*********** FIM DO ANALISADOR LÉXICO ***********>

// Protótipo das Funções do Analisador Sintático

int S();

int Stmt();

int S_Stmt();

int RS_Stmt();

int Expr_Stmt();

int Testlist_star_expr();

int Cmd();

int Exp();

int Namedexpr_Test();

int RNamedexpr_Test();

int Test();

int RTest();

int Or_Test();

int ROr_Test();

int And_Test();

int RAnd_Test();

int Not_Test();

int Comparison();

int RComparison();

int Comp_op();

int IF_Stmt();

int RIF_Stmt();

int Miolo();

int For_Stmt();

int While_Stmt();

int RWhile_Stmt();

int A();

int Func_Def();

int List_Params();

int RList_Params();

// <*********** INICIO DO ANALISADOR SINTÁTICO DESCENDENTE RECURSIVO SEM RETROCESSO ***********>

//Implemente aqui a sua função getToken()

void getToken() {
	tk = 0;
	tklinha = 0;
	tkcoluna = 0;
}

//S -> NEWLINE S | Stmt S | ENDMARKER 
int S(){
	if(NEWLINE()){
		if (S()){
			return 1;
		}
		else{return 0;}
	}
	else if(Stmt()){
		if (S()){
			return 1;
		}
		else{return 0;}
	}
	else if (ENDMARKER()){
		return 1;
	}
	else{return 0;}
}

//Stmt -> S_Stmt | C_Stmt | Cmd Stmt | ? 
int Stmt(){
	if (S_Stmt()){
		return 1;
	}
	else if (C_Stmt()){
		return 1;
	}
	else if(Cmd()){
		if (Stmt()){
			return 1;
		}
		else{return 0;}
	}
	else {return 1;}
}

//S_Stmt -> Expr_Stmt RS_Stmt 
int S_Stmt(){
	if(Expr_Stmt()){
		if (RS_Stmt()){
			return 1;
		}
		else{return 0;}
	}
	else{return 0;}
}

//RS_Stmt -> ; S_Stmt | RS_Stmt | ; NEWLINE | NEWLINE 
int RS_Stmt(){
	if(tk == TK1){// ;
		getToken(); 
		if (S_Stmt()){
			return 1;
		}
		else{return 0;}
	}
	else if (RS_Stmt()){
		return 1;
	}
	else if(tk == TK1){// ;
		getToken(); 
		if (NEWLINE()){
			return 1;
		}
		else{return 0;}
	}
	else if (NEWLINE()){
		return 1;
	}
	else{return 0;}
}

//Expr_Stmt -> Testlist_star_expr ... 
int Expr_Stmt(){
	if(Testlist_star_expr()){
		if(tk == TK2){// ...
			getToken();
			return 1;
		}
		else{return 0;}
	}
	else{return 0;}
}

//Testlist_star_expr -> (Test) | (Star_Expr) | (, (Test)) Testlist_star_expr | (, (Star_Expr)) Testlist_star_expr, | , 
int Testlist_star_expr(){
	if(tk == TK3){// (Test)
		getToken();
		return 1;
	}
	else if(tk == TK4){// (Star_Expr)
		getToken();
		return 1;
	}
	else if(tk == TK5){// (,
		getToken(); 
		if(tk == TK6){// (Test))
			getToken();
			if (Testlist_star_expr()){
				return 1;
			}
			else{return 0;}
		}
		else{return 0;}
	}
	else if(tk == TK5){// (,
		getToken(); 
		if(tk == TK7){// (Star_Expr))
			getToken();
			if (Testlist_star_expr,()){
				return 1;
			}
			else{return 0;}
		}
		else{return 0;}
	}
	else if(tk == TK8){// ,
		getToken();
		return 1;
	}
	else{return 0;}
}

//Cmd -> Exp | IF_Cmd | FOR_Cmd | WHILE_Cmd 
int Cmd(){
	if (Exp()){
		return 1;
	}
	else if (IF_Cmd()){
		return 1;
	}
	else if (FOR_Cmd()){
		return 1;
	}
	else if (WHILE_Cmd()){
		return 1;
	}
	else{return 0;}
}

//Exp -> Exp + id | id | (Exp) 
int Exp(){
	if(Exp()){
		if(tk == TK9){// +
			getToken();
			if(tk == TK10){// id
				getToken();
				return 1;
			}
			else{return 0;}
		}
		else{return 0;}
	}
	else if(tk == TK10){// id
		getToken();
		return 1;
	}
	else if(tk == TK11){// (Exp)
		getToken();
		return 1;
	}
	else{return 0;}
}

//Namedexpr_Test -> Test RNamedexpr_Test 
int Namedexpr_Test(){
	if(Test()){
		if (RNamedexpr_Test()){
			return 1;
		}
		else{return 0;}
	}
	else{return 0;}
}

//RNamedexpr_Test -> Test := Test | ? 
int RNamedexpr_Test(){
	if(Test()){
		if(tk == TK12){// :=
			getToken();
			if (Test()){
				return 1;
			}
			else{return 0;}
		}
		else{return 0;}
	}
	else {return 1;}
}

//Test -> Or_Test RTest 
int Test(){
	if(Or_Test()){
		if (RTest()){
			return 1;
		}
		else{return 0;}
	}
	else{return 0;}
}

//RTest -> if Or_Test else Test | ? 
int RTest(){
	if(tk == TK13){// if
		getToken(); 
		if (Or_Test()){
			if(tk == TK14){// else
				getToken();
				if (Test()){
					return 1;
				}
				else{return 0;}
			}
			else{return 0;}
		}
		else{return 0;}
	}
	else {return 1;}
}

//Or_Test -> And_Test ROr_Test 
int Or_Test(){
	if(And_Test()){
		if (ROr_Test()){
			return 1;
		}
		else{return 0;}
	}
	else{return 0;}
}

//ROr_Test -> or Or_Test | ? 
int ROr_Test(){
	if(tk == TK15){// or
		getToken(); 
		if (Or_Test()){
			return 1;
		}
		else{return 0;}
	}
	else {return 1;}
}

//And_Test -> Not_Test RAnd_Test 
int And_Test(){
	if(Not_Test()){
		if (RAnd_Test()){
			return 1;
		}
		else{return 0;}
	}
	else{return 0;}
}

//RAnd_Test -> and And_Test | ? 
int RAnd_Test(){
	if(tk == TK16){// and
		getToken(); 
		if (And_Test()){
			return 1;
		}
		else{return 0;}
	}
	else {return 1;}
}

//Not_Test -> not Not_Test | Comparison 
int Not_Test(){
	if(tk == TK17){// not
		getToken(); 
		if (Not_Test()){
			return 1;
		}
		else{return 0;}
	}
	else if (Comparison()){
		return 1;
	}
	else{return 0;}
}

//Comparison -> Expr RComparison 
int Comparison(){
	if(Expr()){
		if (RComparison()){
			return 1;
		}
		else{return 0;}
	}
	else{return 0;}
}

//RComparison -> Comp_op Expr RComparison | ? 
int RComparison(){
	if(Comp_op()){
		if (Expr()){
			if (RComparison()){
				return 1;
			}
			else{return 0;}
		}
		else{return 0;}
	}
	else {return 1;}
}

//Comp_op -> 2Linha | 2Linha | 2Linha | 2Linha | 2Linha | 2Linha | 2Linha | 2Linha | 2Linha 2Linha | 2Linha | 2Linha 2Linha 
int Comp_op(){
	if(tk == TK18){// '<'
		getToken();
		return 1;
	}
	else if(tk == TK19){// '>'
		getToken();
		return 1;
	}
	else if(tk == TK20){// '=='
		getToken();
		return 1;
	}
	else if(tk == TK21){// '>='
		getToken();
		return 1;
	}
	else if(tk == TK22){// '<='
		getToken();
		return 1;
	}
	else if(tk == TK23){// '<>'
		getToken();
		return 1;
	}
	else if(tk == TK24){// '!='
		getToken();
		return 1;
	}
	else if(tk == TK25){// 'in'
		getToken();
		return 1;
	}
	else if(tk == TK26){// 'not'
		getToken(); 
		if(tk == TK25){// 'in'
			getToken();
			return 1;
		}
		else{return 0;}
	}
	else if(tk == TK27){// 'is'
		getToken();
		return 1;
	}
	else if(tk == TK27){// 'is'
		getToken(); 
		if(tk == TK26){// 'not'
			getToken();
			return 1;
		}
		else{return 0;}
	}
	else{return 0;}
}

//IF_Stmt -> if Namedexpr_Test : Miolo RIF_Stmt 
int IF_Stmt(){
	if(tk == TK13){// if
		getToken(); 
		if (Namedexpr_Test()){
			if(tk == TK28){// :
				getToken();
				if (Miolo()){
					if (RIF_Stmt()){
						return 1;
					}
					else{return 0;}
				}
				else{return 0;}
			}
			else{return 0;}
		}
		else{return 0;}
	}
	else{return 0;}
}

//RIF_Stmt -> elif Namedexpr_Test : Miolo RIF_Stmt | else : MIOLO | ? 
int RIF_Stmt(){
	if(tk == TK29){// elif
		getToken(); 
		if (Namedexpr_Test()){
			if(tk == TK28){// :
				getToken();
				if (Miolo()){
					if (RIF_Stmt()){
						return 1;
					}
					else{return 0;}
				}
				else{return 0;}
			}
			else{return 0;}
		}
		else{return 0;}
	}
	else if(tk == TK14){// else
		getToken(); 
		if(tk == TK28){// :
			getToken();
			if (MIOLO()){
				return 1;
			}
			else{return 0;}
		}
		else{return 0;}
	}
	else {return 1;}
}

//Miolo -> NEWLINE ident Cmd dedent 
int Miolo(){
	if(NEWLINE()){
		if(tk == TK30){// ident
			getToken();
			if (Cmd()){
				if(tk == TK31){// dedent
					getToken();
					return 1;
				}
				else{return 0;}
			}
			else{return 0;}
		}
		else{return 0;}
	}
	else{return 0;}
}

//For_Stmt -> for id in id: Miolo 
int For_Stmt(){
	if(tk == TK32){// for
		getToken(); 
		if(tk == TK10){// id
			getToken();
			if(tk == TK33){// in
				getToken();
				if(tk == TK34){// id:
					getToken();
					if (Miolo()){
						return 1;
					}
					else{return 0;}
				}
				else{return 0;}
			}
			else{return 0;}
		}
		else{return 0;}
	}
	else{return 0;}
}

//While_Stmt -> while Namedexpr_Test : Miolo RWhile_Stmt 
int While_Stmt(){
	if(tk == TK35){// while
		getToken(); 
		if (Namedexpr_Test()){
			if(tk == TK28){// :
				getToken();
				if (Miolo()){
					if (RWhile_Stmt()){
						return 1;
					}
					else{return 0;}
				}
				else{return 0;}
			}
			else{return 0;}
		}
		else{return 0;}
	}
	else{return 0;}
}

//RWhile_Stmt -> else : Miolo | ? 
int RWhile_Stmt(){
	if(tk == TK14){// else
		getToken(); 
		if(tk == TK28){// :
			getToken();
			if (Miolo()){
				return 1;
			}
			else{return 0;}
		}
		else{return 0;}
	}
	else {return 1;}
}

//A -> Cmd RP 
int A(){
	if(Cmd()){
		if (RP()){
			return 1;
		}
		else{return 0;}
	}
	else{return 0;}
}

//Func_Def -> def nome (List_Params) : Miolo 
int Func_Def(){
	if(tk == TK36){// def
		getToken(); 
		if(tk == TK37){// nome
			getToken();
			if(tk == TK38){// (List_Params)
				getToken();
				if(tk == TK28){// :
					getToken();
					if (Miolo()){
						return 1;
					}
					else{return 0;}
				}
				else{return 0;}
			}
			else{return 0;}
		}
		else{return 0;}
	}
	else{return 0;}
}

//List_Params -> id RList_Params 
int List_Params(){
	if(tk == TK10){// id
		getToken(); 
		if (RList_Params()){
			return 1;
		}
		else{return 0;}
	}
	else{return 0;}
}

//RList_Params -> , id RList_Params | ? 
int RList_Params(){
	if(tk == TK8){// ,
		getToken(); 
		if(tk == TK10){// id
			getToken();
			if (RList_Params()){
				return 1;
			}
			else{return 0;}
		}
		else{return 0;}
	}
	else {return 1;}
}

// <*********** FIM DO ANALISADOR SINTÁTICO DESCENDENTE RECURSIVO SEM RETROCESSO ***********>

int main(){
}