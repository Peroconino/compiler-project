%option noyywrap
%option nodefault
%option outfile="lexer.c" header-file="lexer.h"

%{
    #include "exp.h"
%}

LETRA     [A-Za-z_]
DIGITO    [0-9]
ID        {LETRA}({LETRA}|{DIGITO})*
DIGITOS   {DIGITO}+
FRACAO    \.{DIGITOS}
EXPOENTE  (E[+\-]?{DIGITOS})?
NUM       {DIGITOS}{FRACAO}?{EXPOENTE}?

%%

[ \t\r]+    { /* Ignora espaços */ }
\n          { /* Conta linhas */ }
"//".*      { /* Comentário de linha */ }
"/*"([^*]|(\*+[^*/]))*"*/" { /* Comentário de bloco */ }
\+          { return token(TOK_OP, SOMA); }
-           { return token(TOK_OP, SUB); }
\*          { return token(TOK_OP, MULT); }
\/          { return token(TOK_OP, DIV); }
\(          { return token(TOK_PONT, PARESQ); }
\)          { return token(TOK_PONT, PARDIR); }
\<=         { return token(TOK_RELOP, LE); }
>=          { return token(TOK_RELOP, GE); }
=           { return token(TOK_RELOP, EQ); }
\<          { return token(TOK_RELOP,LT);}
>           { return token(TOK_RELOP, GT); }
:=          { return token(ASSIGMENT, NONE); }
if          { return token(IF, NONE); }
then        { return token(THEN, NONE); }
else        { return token(ELSE, NONE); }
while       { return token(WHILE, NONE); }
repeat      { return token(REPEAT, NONE); }
until       { return token(UNTIL, NONE); }
{NUM}       { float val = atof(yytext); return token(TOK_NUM, &val); }
{ID}        { return token(TOK_ID, yytext); }
<<EOF>>     { return token(TOK_EOF, NULL); }
.           { return token(TOK_ERRO, 0); }

%%

Token tok;
Token *token(int tipo, void *valor) {
    static Token tok;  // 'static' evita que a variável seja destruída após o retorno
 
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

    return &tok;
}
