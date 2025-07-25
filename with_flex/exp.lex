%option noyywrap
%option nodefault
%option outfile="lexer.c" header-file="lexer.h"

%{
    #include "exp.h"
    int cont_linha = 0;
    int cont_coluna = 0;
%}

LETRA     [A-Za-z_]
DIGITO    [0-9]
ID        {LETRA}({LETRA}|{DIGITO})*
DIGITOS   {DIGITO}+
FRACAO    \.{DIGITOS}
EXPOENTE  (E[+\-]?{DIGITOS})?
NUM       {DIGITOS}{FRACAO}?{EXPOENTE}?

%%

[ \t\r]+    { cont_coluna++; }
\n          { cont_linha++; }
"//".*      { /*não faz nada*/ }
"/*"([^*]|(\*+[^*/]))*"*/" { /*não faz nada*/ }
\+          { return token(TOK_OP, SOMA); }
-           { return token(TOK_OP, SUB); }
\*          { return token(TOK_OP, MULT); }
\^          { return token(TOK_OP, EXP); }
\/          { return token(TOK_OP, DIV); }
\(          { return token(TOK_PONT, PARESQ); }
\)          { return token(TOK_PONT, PARDIR); }
\<=         { return token(TOK_RELOP, LE); }
>=          { return token(TOK_RELOP, GE); }
==          { return token(TOK_RELOP, EQ); }
!=          { return token(TOK_RELOP, NE); }
\<          { return token(TOK_RELOP,LT);}
>           { return token(TOK_RELOP, GT); }
->          { return token(ASSIGMENT, NONE); }
,           { return token(MUL_VARS, NONE); }
;           { return token(END_EXP, NONE); }
main        { return token(MAIN, NONE); }
inicio      { return token(BEGIN, NONE); }
fim         { return token(END, NONE); }
caso        { return token(IF, NONE); }
entao       { return token(THEN, NONE); }
senao       { return token(ELSE, NONE); }
enquanto    { return token(WHILE, NONE); }
faca        { return token(DO, NONE); }
repita      { return token(REPEAT, NONE); }
ate         { return token(UNTIL, NONE); }
char        { return token(CHAR, NONE); }
int         { return token(INT, NONE); }
float       { return token(FLOAT, NONE); }
{NUM}       { float val = atof(yytext); return token(TOK_NUM, &val); }
{ID}        { return token(TOK_ID, yytext); }
<<EOF>>     { return token(TOK_EOF, NULL); }
.           { return token(TOK_ERRO, 0); }

%%

Token tok;
Token *token(int tipo,void *valor) {
    static Token tok;
 
    tok.tipo = tipo;

    if (tipo == TOK_ID ) {
        // Faz uma cópia permanente da string
        tok.atributo.valor_str = strdup((char *)valor);
    }
    else if (tipo == TOK_NUM) {
        tok.atributo.valor_float = *((float *)valor);
    }
    else {
        tok.atributo.valor_int = (int)(long)valor;
    }

    tok.posicao.linha = cont_linha;
    tok.posicao.coluna = cont_coluna;

    return &tok;
}
