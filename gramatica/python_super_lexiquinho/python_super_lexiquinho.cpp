
#define foreach(item, array) \
    for(int keep = 1, \
            count = 0,\
            size = sizeof (array) / sizeof *(array); \
        keep && count != size; \
        keep = !keep, count++) \
      for(item = (array) + count; keep; keep = !keep)

#define TKId       1
#define TKInteger  2
#define TKFloat    3
#define TKString   4
#define TKIndent   5
#define TKDedent   6

// Reserved Words
#define TKFalse    7
#define TKClass    8
#define TKFinally  9
#define TKIs       10
#define TKReturn   11
#define TKNone     12
#define TKContinue 13
#define TKFor      14
#define TKLambda   15
#define TKTry      16
#define TKTrue     17
#define TKDef      18
#define TKFrom     19
#define TKNonlocal 20
#define TKWhile    21
#define TKAnd      22
#define TKDel      23
#define TKGlobal   24
#define TKNot      25
#define TKWith     26
#define TKAs       27
#define TKElif     28
#define TKIf       29
#define TKOr       30
#define TKYield    31
#define TKAssert   32
#define TKElse     33
#define TKImport   34
#define TKPass     35
#define TKBreak    36
#define TKExcept   37
#define TKIn       38
#define TKRaise    39

// Operators
#define TKMais            40 // +
#define TKMenos           41 // -
#define TKAsterisco       42 // *
#define TKExpoenten       43 // **
#define TKBarra           44 // /
#define TKDuploBarra      45 // //
#define TKPorcentagem     46 // %
#define TKArroba          47 // @
#define TKLeftShift       48 // <<
#define TKRightShift      49 // >>
#define TKEBitwise        50 // &
#define TKOrBitwise       51 // |
#define TKXORBitwise      52 // ^
#define TKNotBitwise      53 // ~
#define TKMenorQue        54 // <
#define TKMaiorQue        55 // >
#define TKMenorQueOuIgual 56 // <=
#define TKMaiorQueOuIgual 57 // >=
#define TKIgual           58 // ==
#define TKDiferente       59 // !=

// Delimeters
#define TKAbreParenteses     60 // (
#define TKFechaParenteses    61 // )
#define TKAbreColchetes      62 // [
#define TKFechaColchetes     63 // ]
#define TKAbreChaves         64 // {
#define TKFechaChaves        65 // }
#define TKVirgula            66 // ,
#define TKDoisPontos         67 // :
#define TKPonto              68 // .
#define TKPontoEVirgula      69 // ;
#define TKEscape             70 // @
#define TKAtribui            71 // =
#define TKFlecha             72 // ->
#define TKMaisIgual          73 // +=
#define TKMenosIgual         74 // -=
#define TKMultiplicaIgual    75 // *=
#define TKDivideIgual        76 // /=
#define TKDivideInteiroIgual 77 // //=
#define TKPorcentagemIgual   78 // %=
#define TKArrobaIgual        79 // @=
#define TKEBitwiseIgual      80 // &=
#define TKOrBitwiseIgual     81 // |=
#define TKXORBitwiseIgual    82 // ^=
#define TKRightShiftIgual    83 // >>=
#define TKLeftShiftIgual     84 // <<=
#define TKExpoenteIgual      85 // **=
#define TKFim                86 // FIM

#define MAX_COD 1000

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int pos = 0, indent = 0, create_indent = 0, indent_max = 0, on_this = 0, line = 1, col = 0, last_token_kind = 0, si = 0, on_function = 0;
char *text;
FILE *f;

struct pal_res{char palavra[20]; int tk;};

struct token_data
{
    int tk;
    int line;
    int col;
    char *lex;
};

struct token_data *token_list[200], stoken;

struct pal_res reserveds[] = {
    { "False", TKFalse },
    { "class", TKClass },
    { "finally", TKFinally },
    { "is", TKIs },
    { "return", TKReturn },
    { "None", TKNone },
    { "continue", TKContinue },
    { "for", TKFor },
    { "lambda", TKLambda },
    { "try", TKTry },
    { "True", TKTrue },
    { "def", TKDef },
    { "from", TKFrom },
    { "nonlocal", TKNonlocal },
    { "while", TKWhile },
    { "and", TKAnd },
    { "del", TKDel },
    { "global", TKGlobal },
    { "not", TKNot },
    { "with", TKWith },
    { "as", TKAs },
    { "elif", TKElif },
    { "if", TKIf },
    { "or", TKOr },
    { "yield", TKYield },
    { "assert", TKAssert },
    { "else", TKElse },
    { "import", TKImport },
    { "pass", TKPass },
    { "break", TKBreak },
    { "except", TKExcept },
    { "in", TKIn },
    { "raise", TKRaise },
    { "fimtabela", TKId }
};

struct pal_res op_and_delimeters[] = {
    { "+", TKMais },
    { "-", TKMenos },
    { "*", TKAsterisco },
    { "**", TKExpoenten },
    { "/", TKBarra },
    { "//", TKDuploBarra },
    { "%%", TKPorcentagem },
    { "@", TKArroba },
    { "<", TKMenorQue },
    { ">", TKMaiorQue },
    { "&", TKEBitwise },
    { "|", TKOrBitwise },
    { "^", TKXORBitwise },
    { "~", TKNotBitwise },
    { "<=", TKMenorQueOuIgual },
    { ">=", TKMaiorQueOuIgual },
    { "==", TKIgual },
    { "!=", TKDiferente },
    { "(", TKAbreParenteses },
    { ")", TKFechaParenteses },
    { "[", TKAbreColchetes },
    { "]", TKFechaColchetes },
    { "{}", TKAbreChaves },
    { "}", TKFechaChaves },
    { ",", TKVirgula },
    { ".", TKPonto },
    { ";", TKPontoEVirgula },
    { "@", TKEscape },
    { "=", TKAtribui },
    { "->", TKFlecha },
    { "+=", TKMaisIgual },
    { "-=", TKMenosIgual },
    { "*=", TKMultiplicaIgual },
    { "/=", TKDivideIgual },
    { "//=", TKDivideInteiroIgual },
    { "%%=", TKPorcentagemIgual },
    { "@=", TKArrobaIgual },
    { "&=", TKEBitwiseIgual },
    { "|=", TKOrBitwiseIgual },
    { "^=", TKXORBitwiseIgual },
    { ">>=", TKRightShiftIgual },
    { "<<=", TKLeftShiftIgual },
    { "**=", TKExpoenteIgual },
    { "<<", TKLeftShift },
    { ">>", TKRightShift }
};

char *tokens[87] = {
    "", "TKId", "TKInteger", "TKFloat", "TKString", "TKIndent", "TKDedent", "TKFalse", "TKClass",
    "TKFinally", "TKIs", "TKReturn", "TKNone", "TKContinue", "TKFor", "TKLambda", "TKTry",
    "TKTrue", "TKDef", "TKFrom", "TKNonlocal", "TKWhile", "TKAnd", "TKDel", "TKGlobal",
    "TKNot", "TKWith", "TKAs", "TKElif", "TKIf", "TKOr", "TKYield", "TKAssert", "TKElse",
    "TKImport", "TKPass", "TKBreak", "TKExcept", "TKIn", "TKRaise", "TKMais", "TKMenos",
    "TKAsterisco", "TKExpoenten", "TKBarra", "TKDuploBarra", "TKPorcentagem", "TKArroba",
    "TKLeftShift", "TKRightShift", "TKEBitwise", "TKOrBitwise", "TKXORBitwise", "TKNotBitwise",
    "TKMenorQue", "TKMaiorQue", "TKMenorQueOuIgual", "TKMaiorQueOuIgual", "TKIgual", "TKDiferente",
    "TKAbreParenteses", "TKFechaParenteses", "TKAbreColchetes", "TKFechaColchetes", "TKAbreChaves",
    "TKFechaChaves", "TKVirgula", "TKDoisPontos", "TKPonto", "TKPontoEVirgula", "TKEscape",
    "TKAtribui", "TKFlecha", "TKMaisIgual", "TKMenosIgual", "TKMultiplicaIgual", "TKDivideIgual",
    "TKDivideInteiroIgual", "TKPorcentagemIgual", "TKArrobaIgual", "TKEBitwiseIgual",
    "TKOrBitwiseIgual", "TKXORBitwiseIgual", "TKRightShiftIgual", "TKLeftShiftIgual", "TKExpoenteIgual",
    "TKFim"
};

int palavra_reservada(char lex[]) {
    int postab=0;

    while (strcmp("fimtabela", reserveds[postab].palavra)!=0) {
        if (strcmp(lex,reserveds[postab].palavra)==0)
            return reserveds[postab].tk;
        postab++;
    }

    return TKId;
}


int find_operators_and_delimeters(char lex[], int* pos) {
    for (int i = 2; i >= 0; i--) {
        foreach (pal_res *sym, op_and_delimeters) {
            int b = 1;
            for (int j = i; j >= 0 && b > 0; j--)
                if (lex[j] != sym->palavra[j])
                    b = 0;
            if (b == 1) {
                *pos += (i + 1);
                return sym->tk;
            }
        }
    }

    return 0;
}
// int isDelimitador(char lex[]) {

// }

int check_indent() {
    if(create_indent && indent == indent_max) {
        indent++;
        indent_max++;
        create_indent = 0;
        return TKIndent;
    }
    if(indent > indent_max) {
        fprintf(f, "Erro léxico: identação inesperada na linha %d\n", line);
        return 0;
    }
    indent++;
    return 0;
}

int end_line_fix_indent() {
    if(create_indent && on_this == 0) {
        fprintf(f, "Erro léxico: identação incorreta na linha %d\n", line);
        create_indent = 0;
        return 0;
    }
    if(indent < indent_max) {
        indent_max = indent;
        indent = 0;
        on_this = 0;
        return 1;
    }
    indent = 0;
    on_this = 0;
    return 0;
}

int rec_equ(char st[],char lex[]) {
    int estado = 0, fim = 0, posl = 0, tk_tmp = 0, num_s = 0, err = 0, pos_tmp = 0;

    while (!fim) {
        col++;
        char c=st[pos];

        lex[posl++]=c;
        
        switch(estado) {
            case 0:
                num_s = 0;
                if (c >= '0' && c <= '9') {
                    if(c == '0') num_s = 1;
                    pos++;
                    estado = 2;
                    break;
                }
                if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_') {
                    pos++;
                    estado = 1;
                    break;
                }

                if (c == '.' && (st[pos+1] >= '0' && st[pos+1] <= '9')) {
                    pos++;
                    num_s = 0;
                    estado = 3;
                    break;
                }

                if (c == '=') {
                    c = st[pos + 1];

                    if(c == '=') {
                        lex[posl++]=c;
                        lex[posl] = '\0';
                        pos+= 2;
                        return TKIgual;
                    }

                    lex[posl] = '\0';
                    pos++;
                    return TKAtribui;
                }

                if (c == '"') {
                    pos++;
                    estado = 4;
                    break;
                }

                char lex_tmp[4];
                lex_tmp[0] = st[pos];
                lex_tmp[1] = st[pos + 1];
                lex_tmp[2] = st[pos + 2];
                lex_tmp[3] = '\0';

                pos_tmp = pos + 1;
                tk_tmp = find_operators_and_delimeters(lex_tmp, &pos);
                if(tk_tmp) {
                    for(; pos_tmp < pos; pos_tmp++) {
                        lex[posl++] = st[pos_tmp];
                    }
                    lex[posl]='\0';
                    return tk_tmp;
                }


                if (c=='\t') {
                    tk_tmp = check_indent();
                    if(tk_tmp != 0) {
                        lex[posl]='\0';
                        pos++;
                        return tk_tmp;
                    }
                    pos++;posl--; break;
                }

                if (c==':') {
                    lex[posl]='\0';pos++; create_indent = 1; on_this = 1; return TKDoisPontos;
                }
                if (c==' ') {pos++;posl--;break;}
                if (c=='\n') {
                    pos++;posl--;
                    col = 0;
                    strcpy(lex, "\0");
                    line++;
                    if(end_line_fix_indent()) return TKDedent;
                    break;
                }
                if (c=='\0') {end_line_fix_indent(); return -1;}
                fprintf(f, "Erro léxico: encontrou o caracter %c na posição %d\n",c,pos);
                pos++;
                break;

            case 1:
                if (c>='a' && c<='z' || c>='A' && c<='Z' || c=='_' || c>='0' && c<='9') {pos++;break;}
                lex[--posl]='\0';
                return palavra_reservada(lex);

            case 2:
                switch(num_s) {
                    case 0:
                        if (c >= '0' && c <= '9' || c == '_') {
                            pos++;
                        } else if(c == '.' && (st[pos+1] >= '0' && st[pos+1] <= '9')) {
                            pos++;
                            estado = 3;
                            num_s = 0;
                        } else if(c == 'e' || c == 'E') {
                            pos++;
                            estado = 3;
                            num_s = 1;
                        } else {
                            err = 1;
                        }
                        break;
                    case 1:
                        if (c >= '0' && c <= '9' || c == '_') {
                            pos++;
                            num_s = 0;
                        } else if (c == 'b' || c == 'x' || c == 'o') {
                            pos++;
                            num_s = c;
                        } else if(c == '.' && (st[pos+1] >= '0' && st[pos+1] <= '9')) {
                            pos++;
                            estado = 3;
                            num_s = 0;
                        } else if(c == 'e' || c == 'E') {
                            pos++;
                            estado = 3;
                            num_s = 1;
                        } else {
                            err = 1;
                        }
                        break;
                    case 'b': 
                        if (c == '0' || c == '1' || c == '_') {
                            pos++; break;
                        } else {
                            err = 1;
                        }
                        break;
                    case 'o':
                        if (c >= '0' && c <= '7' || c == '_') {
                            pos++; break;
                        } else {
                            err = 1;
                        }
                        break;
                    case 'x': 
                        if ((c >= '0' && c <= '9') || c == '_' || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
                            pos++; break; 
                        } else {
                            err = 1;
                        }
                        break;
                    default:
                        err = 1;
                        break;
                }
                if (err) {
                    lex[--posl]='\0';
                    return TKInteger;
                }
                break;
            case 3:
                err = 0;
                switch(num_s) {
                    case 0:
                        if (c >= '0' && c <= '9' || c == '_') {
                            pos++;
                        } else if((c == 'e' || c == 'E') && lex[posl - 1] != '.') {
                            pos++;
                            num_s = 1;
                        } else {
                            err = 1;
                        }
                        break;
                    case 1:
                        if (c >= '0' && c <= '9' || c == '+' || c == '-') {
                            pos++;
                            num_s = 2;
                        } else {
                            err = 1;
                        }
                        break;
                    case 2:
                        if (c >= '0' && c <= '9' || c == '_' ) {
                            pos++;
                        } else {
                            err = 1;
                        }
                        break;
                }
                if (err) {
                    lex[--posl]='\0';
                    pos--;
                    return TKFloat;
                }
            case 4:
                pos++;
                if(num_s == 0 && c == '"') {
                    lex[posl]='\0';
                    return TKString;
                }
                if(c == '\\' && num_s == 0) num_s = 1;
                else num_s = 0;
        } //switch
    }// while
}// função

void readfile() {
    FILE *f = fopen("sample.py", "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  //same as rewind(f);

    text = (char *)malloc(sizeof(char)*fsize + 1);
    fread(text, fsize, 1, f);
    fclose(f);

    text[fsize] = 0;
}

void le_token(){
    stoken = *token_list[si++];
}

void mostra_t() {
    printf("%s lex=%s na lin %i, col %i\n", tokens[stoken.tk], stoken.lex, stoken.line, stoken.col);
}

void geralabel(char label[]) {
    static int numlabel=0;
    sprintf(label,"LB%03d",numlabel++);
}

void geratemp(char temp[]) {
    static int numtemp=0;
    sprintf(temp,"T%03d",numtemp++);
}

/****************/
/*              */
/*  EXPRESSÔES  */
/*              */
/****************/

int T(char T_p[MAX_COD],char T_c[MAX_COD]);
int E(char E_p[MAX_COD],char E_c[MAX_COD]);
int R(char R_hp[MAX_COD],char R_sp[MAX_COD],char R_hc[MAX_COD],char R_sc[MAX_COD]);
int F(char F_p[MAX_COD],char F_c[MAX_COD]);
int S(char S_hp[MAX_COD],char S_sp[MAX_COD],char S_hc[MAX_COD],char S_sc[MAX_COD]);
int Rel(char Rel_p[MAX_COD],char Rel_c[MAX_COD]);
int CallF(char C_p[MAX_COD], char C_c[MAX_COD], char func_id[MAX_COD]);

int A(char A_p[MAX_COD],char A_c[MAX_COD]) {
    char A1_p[MAX_COD],A1_c[MAX_COD],Rel_p[MAX_COD],Rel_c[MAX_COD]; 
    char id[100];

    if (!Rel(Rel_p,Rel_c)) return 0;

    if (stoken.tk!=TKAtribui){
        strcpy(A_p,Rel_p);
        strcpy(A_c,Rel_c);

        return 1;
    }

    le_token(); // consome o sinal de atribuição

    if (!A(A1_p,A1_c)) return 0;

    if(si < 2 || token_list[si-2]->tk != TKString)
        sprintf(A_c,"%s\t%s=%s\n",A1_c,Rel_p,A1_p);
    else
        sprintf(A_c, "%s\n", A1_c);

    strcpy(A_p,Rel_p);

    return 1;   
}

int CallF(char C_p[MAX_COD], char C_c[MAX_COD], char func_id[MAX_COD]) {
    char F_c[MAX_COD], F_p[MAX_COD];
    int params = 1;

    le_token();
    strcpy(C_c, "");

    while(stoken.tk != TKFechaParenteses) {
        if(!Rel(F_p, F_c)) return 0;

        sprintf(C_c, "%s%s\tParam %s\n", C_c, F_c, F_p);

        if(stoken.tk != TKVirgula && stoken.tk != TKFechaParenteses) return 0;
        if(stoken.tk == TKVirgula) le_token();
        params++;
    }

    geratemp(C_p);
    sprintf(C_c, "%s\tParam %s\n", C_c, C_p);

    if(stoken.tk == TKFechaParenteses) {
        le_token();
        sprintf(C_c, "%s\tCall %s,%i\n", C_c, func_id, params);

        return 1;
    }
}

int Rel(char Rel_p[MAX_COD],char Rel_c[MAX_COD]) {
    char E1_c[MAX_COD],E2_c[MAX_COD],R_sc[MAX_COD],E1_p[MAX_COD],E2_p[MAX_COD],R_sp[MAX_COD];
    if (E(E1_p,E1_c)) {
        char op[10];

        if (stoken.tk==TKMaiorQue) strcpy(op,">");
        else if (stoken.tk==TKMenorQue) strcpy(op,"<");
        else if (stoken.tk==TKIgual) strcpy(op,"==");
        else if (stoken.tk==TKDiferente) strcpy(op,"<>");
        else if (stoken.tk==TKMaiorQueOuIgual) strcpy(op,">=");
        else if (stoken.tk==TKMenorQueOuIgual) strcpy(op,"<=");

        if (stoken.tk==TKMaiorQue||stoken.tk==TKMenorQue||stoken.tk==TKIgual||stoken.tk==TKDiferente||stoken.tk==TKMaiorQueOuIgual||stoken.tk==TKMenorQueOuIgual) {
            le_token();
            if (E(E2_p,E2_c)) {
                geratemp(Rel_p);
                sprintf(Rel_c,"%s%s\t%s = %s%s%s\n",E1_c,E2_c,Rel_p,E1_p,op,E2_p);
                return 1;
            }  
            return 0;
        }
        else {
            strcpy(Rel_c,E1_c);
            strcpy(Rel_p,E1_p);
            return 1;
        }
    }
    return 0;
}

int E(char E_p[MAX_COD],char E_c[MAX_COD]) {
    char T_p[MAX_COD],T_c[MAX_COD],R_hp[MAX_COD],R_sp[MAX_COD],R_hc[MAX_COD],R_sc[MAX_COD];

    if (T(T_p,T_c)) {
        strcpy(R_hc,T_c);
        strcpy(R_hp,T_p);

        if (R(R_hp,R_sp,R_hc,R_sc)) {
            strcpy(E_c,R_sc);
            strcpy(E_p,R_sp);

            return 1;
        }
    }
    return 0;
}

int R(char R_hp[MAX_COD],char R_sp[MAX_COD],char R_hc[MAX_COD],char R_sc[MAX_COD]) {
    char T_c[MAX_COD],R1_hc[MAX_COD],R1_sc[MAX_COD],T_p[MAX_COD],R1_hp[MAX_COD],R1_sp[MAX_COD];

    if (stoken.tk==TKMais) {
        le_token();
        if (T(T_p,T_c)) {
            geratemp(R1_hp);
            sprintf(R1_hc,"%s%s\t%s=%s+%s\n",R_hc,T_c,R1_hp,R_hp,T_p);

            if (R(R1_hp,R1_sp,R1_hc,R1_sc)) {
                strcpy(R_sp,R1_sp);
                strcpy(R_sc,R1_sc);
                return 1;
            }
        }
        return 0;
    }
 
    if (stoken.tk==TKMenos) {   
        le_token();

        if (T(T_p,T_c)) {
            geratemp(R1_hp);
            sprintf(R1_hc,"%s%s\t%s=%s-%s\n",R_hc,T_c,R1_hp,R_hp,T_p);

            if (R(R1_hp,R1_sp,R1_hp,R1_sp)) {
                strcpy(R_sp,R1_sp);
                strcpy(R_sc,R1_sc);
                return 1;
            }
        }
        return 0;
    }
    strcpy(R_sp,R_hp);
    strcpy(R_sc,R_hc);
    return 1;
}

int T(char T_p[MAX_COD],char T_c[MAX_COD]) {
    char F_c[MAX_COD],F_p[MAX_COD],S_hp[MAX_COD],S_sp[MAX_COD],S_hc[MAX_COD],S_sc[MAX_COD];
    if (F(F_p,F_c)) {
        strcpy(S_hc,F_c);
        strcpy(S_hp,F_p);

        if (S(S_hp,S_sp,S_hc,S_sc)) {
            strcpy(T_c,S_sc);
            strcpy(T_p,S_sp);

            return 1;
        }
    }
    return 0;
}

int S(char S_hp[MAX_COD],char S_sp[MAX_COD],char S_hc[MAX_COD],char S_sc[MAX_COD]) {
    char F_c[MAX_COD],S1_hc[MAX_COD],S1_sc[MAX_COD],F_p[MAX_COD],S1_hp[MAX_COD],S1_sp[MAX_COD];
    if (stoken.tk == TKAsterisco || stoken.tk == TKBarra) {   
        char lex = (stoken.tk == TKAsterisco) ? '*' : '/';
        le_token();

        if (F(F_p,F_c)) {
            geratemp(S1_hp);
            sprintf(S1_hc,"%s%s\t%s=%s %c %s\n",S_hc,F_c,S1_hp,S_hp,lex,F_p);

            if (S(S1_hp,S1_sp,S1_hc,S1_sc)) {
                strcpy(S_sp,S1_sp);
                strcpy(S_sc,S1_sc);
                return 1;
            }
        }
        return 0;
    }
    strcpy(S_sp,S_hp);
    strcpy(S_sc,S_hc);
    return 1;
}

int F(char F_p[MAX_COD],char F_c[MAX_COD]) {
    if (stoken.tk==TKString) {
        geratemp(F_p);
        sprintf(F_c,"\n\t%s = addr %s\n", F_p, token_list[si-3]->lex);

        char string[strlen(stoken.lex) + 1], control[10];
        strcpy(string, stoken.lex);
        geratemp(control);
        int i = 0;

        sprintf(F_c, "%s\t%s=0\n", F_c, control);

        foreach(char *item, string) {
            if(*item == '"') continue;

            char tmp[10];
            geratemp(tmp);

            if(*item == '\0')
                sprintf(F_c, "%s\t%s='\\0'\n", F_c, tmp);
            else if(*item == '\n')
                sprintf(F_c, "%s\t%s='\\n'\n", F_c, tmp);
            else if(*item == '\t')
                sprintf(F_c, "%s\t%s='\\t'\n", F_c, tmp);
            else
                sprintf(F_c, "%s\t%s='%c'\n", F_c, tmp, *item);

            sprintf(F_c, "%s\t%s[%s]=%s\n", F_c, F_p, control, tmp);

            if(*item != '\0') sprintf(F_c, "%s\t%s=%s+4\n", F_c, control, control);
        }

        le_token();

        return 1;
    }
    if (stoken.tk==TKInteger || stoken.tk==TKFloat) {
        geratemp(F_p);
        sprintf(F_c,"\t%s = %s\n", F_p, stoken.lex);
        le_token();

        return 1;
    }
    if (stoken.tk==TKTrue) {
        geratemp(F_p);
        sprintf(F_c,"\t%s = 1\n", F_p);
        le_token();

        return 1;
    }
    if (stoken.tk==TKFalse) {
        geratemp(F_p);
        sprintf(F_c,"\t%s = 0\n", F_p);
        le_token();

        return 1;
    }
    if (stoken.tk==TKId) {
        strcpy(F_c,"");
        strcpy(F_p, stoken.lex);

        le_token();

        if(stoken.tk==TKAbreParenteses) {
            char C_p[MAX_COD], C_c[MAX_COD];

            if(!CallF(C_p, C_c, F_p)) return 0;

            strcpy(F_c, C_c);
            strcpy(F_p, C_p);
            return 1;
        }

        return 1;
    }
    if (stoken.tk==TKAbreParenteses) {
        char E_c[MAX_COD],E_p[MAX_COD];
        le_token();

        if (E(E_p,E_c))
            if (stoken.tk==TKFechaParenteses) {
                le_token();
                strcpy(F_c,E_c);
                strcpy(F_p,E_p);
                return 1;
            }
        else {
            printf("Erro!!! Esperava fecha parênteses\n");
            return 0;
        }
    }

    return 0;
}

/**************/
/*            */
/*  COMANDOS  */
/*            */
/**************/

int Com(char Com_c[MAX_COD]);

int Com_Composto(char Comp_c[]) {
    // le_token(); // consome o abre_chaves

    strcpy(Comp_c,"");

    while (stoken.tk!=TKDedent && stoken.tk!=TKFim) {
        char Com_C[MAX_COD];
        if (!Com(Com_C)) return 0;
        strcat(Comp_c,Com_C);    
    }

    if(stoken.tk == TKDedent) le_token(); // consome o fecha_chaves
    return 1;
}

int Com_Exp(char Com_c[MAX_COD]) {
    int line = stoken.line;
    char id[10];
    char E_c[MAX_COD],E_p[MAX_COD];
    if (A(E_p,E_c)) {

        if (stoken.line != line) {
            sprintf(Com_c,"%s",E_c);
            return 1;
        }
    }
}

int Com_for(char for_c[]) {
    char Com_c[MAX_COD];
    char labelinicio[10], labelfim[10], controltmp[10];
    char *control_var, *until;
    geralabel(labelinicio);
    geralabel(labelfim);
    geratemp(controltmp);
    le_token();

    if (stoken.tk != TKId) {printf("Esperava um ID"); return 0;}
    control_var = (char *) malloc(sizeof(stoken.lex));
    strcpy(control_var, stoken.lex);
    le_token();

    if (stoken.tk != TKIn) {printf("Esperava a palavra reservada IN\n"); return 0;}
    le_token();

    if (stoken.tk != TKId || (strcmp(stoken.lex, "range") != 0 && strcmp(stoken.lex, "xrange") != 0)) {printf("Esperava a função range(<integer>) ou xrange(<integer>)"); return 0;}
    le_token();

    if (stoken.tk != TKAbreParenteses) {printf("Esperava abrir parenteses"); return 0;}
    le_token();

    if (stoken.tk != TKInteger) {printf("Esperava um inteiro"); return 0;}
    until = (char *) malloc(sizeof(stoken.lex));
    strcpy(until, stoken.lex);
    le_token();

    if (stoken.tk != TKFechaParenteses) {printf("Esperava fechar parenteses"); return 0;}
    le_token();

    if (stoken.tk != TKDoisPontos) {printf("Esperava dois pontos"); return 0;}
    le_token();

    if (stoken.tk != TKIndent) {printf("Esperava identação para o bloco do FOR"); return 0;}
    le_token();

    if (!Com_Composto(Com_c)) {printf("Erro no bloco do FOR."); return 0;}

    sprintf(for_c, "\t%s = 0\n%s:\n\t%s = %s < %s\n\tif %s == 0 goto %s\n%s\t%s = %s + 1\n\tgoto %s\n%s:\n\n", control_var, labelinicio, controltmp, control_var, until, controltmp, labelfim, Com_c, control_var, control_var, labelinicio, labelfim);

    // sprintf(for_c, )

    return 1;
}

int Com_def(char def_c[]) {
    char Rel_c[MAX_COD], Rel_p[MAX_COD], Com_c[MAX_COD], id[MAX_COD];
    int params = 0;
    le_token();

    if (stoken.tk != TKId) {printf("Esperava o ID da função a ser declarada.\n"); return 0;}
    strcpy(id, stoken.lex);
    le_token();

    if (stoken.tk != TKAbreParenteses) {printf("Esperava abrir parenteses"); return 0;}
    le_token();

    while (stoken.tk == TKId) {
        le_token();

        if (stoken.tk != TKFechaParenteses && stoken.tk != TKVirgula) {printf("parametros incorretos"); return 0;}
        if (stoken.tk == TKVirgula) le_token();
    }

    if (stoken.tk != TKFechaParenteses) {printf("Esperava fechar parenteses"); return 0;}
    le_token();

    if (stoken.tk != TKDoisPontos) {printf("Esperava dois pontos na declaração da função.\n"); return 0;}
    le_token();

    if (stoken.tk != TKIndent) {printf("Erro na identação da função.\n"); return 0;}
    le_token();

    if (!Com_Composto(Com_c)) {printf("Erro no bloco do while.\n"); return 0;}

    sprintf(def_c, "\nfunc begin %s\n%sfunc end\n", id, Com_c);
    return 1;
}

int Com_while(char while_c[]) {
    char Rel_c[MAX_COD], Rel_p[MAX_COD], Com_c[MAX_COD];
    char labelinicio[10], labelfim[10];
    geralabel(labelinicio);
    geralabel(labelfim);
    le_token();

    if (!Rel(Rel_p, Rel_c)) {printf("Erro na expressão do while.\n"); return 0;}

    if (stoken.tk != TKDoisPontos) {printf("Esperava dois pontos.\n"); return 0;}
    le_token();

    if (stoken.tk != TKIndent) {printf("Identação incorreta"); return 0;}
    le_token();

    if (!Com_Composto(Com_c)) {printf("Erro no bloco do while.\n"); return 0;}

    sprintf(while_c, "\n%s:\n%s\tif %s==0 goto %s\n%s\tgoto %s\n%s:\n\n", labelinicio, Rel_c, Rel_p, labelfim, Com_c, labelinicio, labelfim);

    return 1;
}

int Com_if(char if_c[]) {
    char Rel_c[MAX_COD], Rel_p[MAX_COD], Com1_c[MAX_COD];
    char labelelse[10], labelfim[10];
    geralabel(labelelse);
    geralabel(labelfim);
    le_token();

    if (Rel(Rel_p, Rel_c))
        if (stoken.tk == TKDoisPontos) {
            le_token();

            if (stoken.tk == TKIndent) {
                le_token();
                char Com1_c[MAX_COD];

                if (Com_Composto(Com1_c)) {

                    if (stoken.tk == TKElse) {
                        le_token();

                        if(stoken.tk == TKDoisPontos) {
                            le_token();

                            if (stoken.tk == TKIndent) {
                                le_token();
                                char Com2_c[MAX_COD];

                                if (Com_Composto(Com2_c)) {
                                    sprintf(if_c, "\n%s\tif %s==0 goto %s\n%s\tgoto %s\n%s:\n%s%s:\n\n", Rel_c, Rel_p, labelelse, Com1_c, labelfim, labelelse, Com2_c, labelfim);
                                    return 1;
                                } else {
                                    printf("Erro no comando do else\n");
                                    return 0;
                                }
                            } else {
                                printf("Identação incorreta");
                                return 0;
                            }
                        } else {
                            printf("Espera dois-pontos");
                            return 0;
                        }
                    } else {
                        // printf("Esperava palavra else\n");
                        // return 0;
                        sprintf(if_c, "\n%s\tif %s==0 goto %s\n%s%s:\n\n", Rel_c, Rel_p, labelfim, Com1_c, labelfim);
                        return 1;
                    }
                } else {
                    printf("Erro no bloco do if\n");
                    return 0;

                }
            } else {
                printf("Identação incorreta \n");
                return 0;
            }

        } else {
            printf("Erro na expressão do if \n");
            return 0;
        } {
            printf("Esperava dois pontos.\n");
            return 0;

        }
}

int Com_return(char return_c[]) {
    char E_p[MAX_COD], E_c[MAX_COD];
    le_token();

    // if(!A(Rel_p, Rel_c)) return 0;
    if (!A(E_p,E_c)) {printf("erro no comando return;");return 0;}

    sprintf(return_c, "%s\treturn %s\n", E_c, E_p);
    return 1;
}

int Com(char Com_c[]) {
    // 
    int cline = stoken.line;

    if (stoken.tk==TKIf) return Com_if(Com_c);
    else if (stoken.tk==TKId) return Com_Exp(Com_c);
    else if (stoken.tk==TKWhile) return Com_while(Com_c);
    else if (stoken.tk==TKFor) return Com_for(Com_c);
    else if (stoken.tk==TKReturn && on_function==1) return Com_return(Com_c);
    else if (stoken.tk==TKDef && on_function==0) {
        int result;

        on_function = 1;
        result = Com_def(Com_c);
        on_function = 0;

        return result;
    }

    //else if (stoken.tk==TKAbreChaves) return Com_Composto(Com_c);

    // printf("oi %s %i %i %s\n", tokens[stoken.tk], stoken.line, cline, stoken.lex);
    else if (stoken.line!=cline) {
        // stoken.tk=le_token();// comando vazio
        strcpy(Com_c,"");
        return 1;    
    }
    else return 0;
}

void reorganize_dedent(int last) {
    int i;

    struct token_data *token;
    for(int i = 0; i < last; i++) {
        token = token_list[i];

        if(token->tk == TKDedent) {
            int j = i - 1;
            struct token_data *aux;

            while(1) {
                aux = token_list[j];
                if(aux->line != token->line) break;

                token_list[j+1] = aux;
                token_list[j] = token;
                j--;
            }
        }
    }
}

int main() {
    readfile();
    int tk;
    char lex[50];
    int itw = 0, itf, i;
    char Com_C[MAX_COD];

    f = fopen("Saida.txt", "w");
    struct token_data *token;

    while ((tk=rec_equ(text,lex))!=-1) {
       // fprintf(f, "Linha: %d Coluna: %i Token: %d(%s) Léxico: %s\n", line, col, tk, tokens[tk], lex);
       token = (struct token_data *) malloc(sizeof(int)*3 + sizeof(lex));

       token->tk = tk;
       token->line = line;
       token->col = col;
       token->lex = (char *) malloc(sizeof(lex));
       strcpy(token->lex, lex);
       if(tk == TKDedent) token->line = line - 1;

       token_list[itw++] = token;
    }
    reorganize_dedent(itw);

    for(i = 0; i < itw; i++) {
        token = token_list[i];
        printf("Linha: %d Coluna: %i Token: %d(%s) Léxico: %s\n", token->line, token->col, token->tk, tokens[token->tk], token->lex);
        fprintf(f, "Linha: %d Coluna: %i Token: %d(%s) Léxico: %s\n", token->line, token->col, token->tk, tokens[token->tk], token->lex);
    }

    token = (struct token_data *) malloc(sizeof(int)*3 + sizeof(lex));

    token->tk = TKFim;
    token->line = line + 2;
    token->col = 0;
    token->lex = (char *) malloc(sizeof("end"));
    strcpy(token->lex, "end");

    token_list[itw++] = token;

    printf("\n\n");
    fprintf(f, "\n\n");

    le_token();
    while(stoken.tk!=TKFim) {
        if(Com(Com_C)==0) {
            printf("Erro no comando. Token %s | Léxico %s | Linha %i | Coluna %i\n", tokens[stoken.tk], stoken.lex, stoken.line, stoken.col);
            fprintf(f, "Erro no comando. Token %s | Léxico %s | Linha %i | Coluna %i\n", tokens[stoken.tk], stoken.lex, stoken.line, stoken.col);
            break;
        }
        else {
            fprintf(f, "%s", Com_C);
            printf("%s", Com_C);
        }
        itf = si;
    }

    fclose(f);
}

