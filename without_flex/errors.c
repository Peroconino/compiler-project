#include "errors.h"
#include <stdio.h>

void trata_erros(Token* tok) {
    if (tok->type != TOKEN_ERROR) {
        return;
    }

    printf("Erro léxico na linha %d, coluna %d: ", tok->line, tok->column);
    printf("\nToken: %s\n", tok->value);

    // Diagnóstico detalhado por ErrorKind
    switch (tok->error) {
        case UNCLOSED_COMMENT:
            printf("Comentário nunca é fechado. Era esperado '*/'.");
            return;
        case INVALID_TOKEN_AFTER_EXCLAMATION:
            printf("Você quis dizer '!='? Era esperado o token '=' após '!'.");
            return;
        case FRACTION_ENDED_WITH_A_DOT:
            printf("Numero mal formado: parte fracionária terminou com ponto.");
            return;
        case ENDED_WITH_E_EXPOENT:
            printf("Expoente terminou com 'e'. Era esperado a continuação da notação científica.");
            return;
        case ENDED_AFTER_EXPOENT_SIGN:
            printf("Expoente terminou com '+' ou '-'. Era esperado dígitos após o sinal.");
            return;
        case UNKNOWN_TOKEN:
            printf("Token não reconhecido pela linguagem.");
            return;
    }

  printf("\n");
}
