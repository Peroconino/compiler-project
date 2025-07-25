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
#define IF 8
#define THEN 9
#define ELSE 10
#define WHILE 11
#define REPEAT 12
#define UNTIL 13
#define NONE 0
#define SOMA 0
#define SUB 1
#define MULT 2
#define DIV 3
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
} Token;
extern Token *token();
extern Token *yylex();

