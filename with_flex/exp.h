#define TRUE 1
#define FALSE 0
#define TOK_NUM 0
#define TOK_OP 1
#define TOK_PONT 2
#define TOK_ERRO 3
#define TOK_RELOP 4
#define TOK_ID 5
#define TOK_EOF 6
#define ASSIGMENT 7
#define MUL_VARS 8
#define END_EXP 9
#define IF 10
#define THEN 11
#define ELSE 12
#define WHILE 13
#define REPEAT 14
#define UNTIL 15
#define MAIN 16
#define BEGIN 17
#define END 18
#define DO 19
#define CHAR 20
#define INT 21
#define FLOAT 22
#define NONE 0
#define SOMA 0
#define SUB 1
#define MULT 2
#define DIV 3
#define EXP 4
#define PARESQ 0
#define PARDIR 1
#define GT 0
#define LT 1
#define LE 2
#define EQ 3
#define NE 4
#define GE 5
typedef struct {
    int tipo;
    union {
        int valor_int;
        float valor_float;
        char *valor_str;
    } atributo;
    union {
        int linha;
        int coluna;
    } posicao;
} Token;
extern Token *token();
extern Token *yylex();

