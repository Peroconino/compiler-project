#include "lexer.h"
#include <stdio.h>

void trata_erros (Token * tok) {
    if(tok->type != TOKEN_ERROR){
        return;
    }

    printf("Erro na linha %d, coluna %d:", tok->line, tok->column);
    printf("\nToken.type: %d",tok->type);
    printf("\nToken.value: %s\n", tok->value);
}
