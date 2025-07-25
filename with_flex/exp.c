#include "lexer.h"
#include "exp.h"

YY_BUFFER_STATE buffer;

void inicializa (char *str) {
	buffer = yy_scan_string(str);
}

Token *proximo_token(){
	return yylex();
}

void imprime_token(Token *tok) {
    if (tok == NULL) {
        printf("Token NULL\n");
        return;
    }

    switch (tok->tipo) {
        case TOK_EOF:
            printf("[EOF] Fim de arquivo\n");
            break;
            
        case TOK_OP:
            switch (tok->atributo.valor_int) {
                case SOMA:  printf("[OP] +\n"); break;
                case SUB:   printf("[OP] -\n"); break;
                case MULT:  printf("[OP] *\n"); break;
                case DIV:   printf("[OP] /\n"); break;
                default:   printf("[OP] Operador desconhecido\n");
            }
            break;

        case TOK_RELOP:
            switch (tok->atributo.valor_int) {
                case LE: printf("[RELOP] <=\n"); break;
                case GE: printf("[RELOP] >=\n"); break;
                case EQ: printf("[RELOP] =\n"); break;
                case LT: printf("[RELOP] <\n"); break;
                case GT: printf("[RELOP] >\n"); break;
                default: printf("[RELOP] Operador relacional desconhecido\n");
            }
            break;

        case TOK_PONT:
            switch (tok->atributo.valor_int) {
                case PARESQ: printf("[PONT] (\n"); break;
                case PARDIR: printf("[PONT] )\n"); break;
                default:     printf("[PONT] Pontuação desconhecida\n");
            }
            break;

        case TOK_NUM:
            printf("[NUM] %.2f\n", tok->atributo.valor_float);
            break;

        case TOK_ID:
            printf("[ID] %s\n", (char *)tok->atributo.valor_str);
            break;

        case IF:
            printf("[PALAVRA-CHAVE] if\n");
            break;

        case THEN:
            printf("[PALAVRA-CHAVE] then\n");
            break;
 
        case ELSE:
            printf("[PALAVRA-CHAVE] else\n");
            break;

        case WHILE:
            printf("[PALAVRA-CHAVE] while\n");
            break;

        case REPEAT:
            printf("[PALAVRA-CHAVE] repeat\n");
            break;

        case UNTIL:
            printf("[PALAVRA-CHAVE] until\n");
            break;

        case ASSIGMENT:
            printf("[ATRIBUIÇÃO] :=\n");
            break;

        case TOK_ERRO:
            printf("[ERRO] Caractere inválido\n");
            break;

        default:
            printf("[DESCONHECIDO] Tipo de token não reconhecido: %d\n", tok->tipo);
    }
}
int main(int argc, char **argv){
	Token *tok;
	char entrada[200];
	printf("\nAnalise lexica da expressao: ");
	fgets(entrada, 200, stdin);
	inicializa(entrada);

	tok = proximo_token();
	while (tok != NULL && tok->tipo != TOK_EOF && tok->tipo != TOK_ERRO) {
		imprime_token(tok);
		tok = proximo_token();
	}
	return 0;
}
