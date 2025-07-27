#include "errors.h"
#include <stdio.h>
#include <string.h>

void trata_erros(Token* tok) {
    if (tok->type != TOKEN_ERROR) {
        return;
    }

    printf("\nErro léxico na linha %d, coluna %d: ", tok->line, tok->column);

    // Diagnóstico detalhado por ErrorKind
    switch (tok->error) {
        case UNCLOSED_COMMENT:
            printf("Comentário nunca é fechado. Era esperado '*/'.\n");
            return;
        case INVALID_TOKEN_AFTER_EXCLAMATION:
            printf("Operador '!' inválido. Você quis dizer '!='?\n");
            return;
        case FRACTION_ENDED_WITH_A_DOT:
            printf("Número mal formatado: parte fracionária terminou com ponto.\n");
            return;
        case ENDED_WITH_E_EXPOENT:
            printf("Expoente terminou com 'e'. Era esperado a continuação da notação científica.\n");
            return;
        case ENDED_AFTER_EXPOENT_SIGN:
            printf("Expoente terminou com '+' ou '-'. Era esperado dígitos após o sinal.\n");
            return;
        case UNKNOWN_TOKEN:
            printf("Token não reconhecido pela linguagem: '%s'\n", tok->value);
            return;
        case NO_ERROR_KIND:
        default:
            break;
    }

    // Diagnóstico extra por valor do token (versão 2)
    if (strlen(tok->value) == 1) {
        switch (tok->value[0]) {
            case '!':
                printf("Operador '!' inválido. Você quis dizer '!='?\n");
                break;
            case '.':
                printf("Número mal formatado ou ponto inesperado\n");
                break;
            default:
                printf("Caractere inválido '%c'\n", tok->value[0]);
        }
    } else if (strchr(tok->value, '.')) {
        printf("Número mal formatado: '%s'\n", tok->value);
    } else if (strchr(tok->value, 'e') || strchr(tok->value, 'E')) {
        printf("Notação científica mal formatada: '%s'\n", tok->value);
    } else {
        printf("Token inválido: '%s'\n", tok->value);
    }
}
