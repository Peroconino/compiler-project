#include "lexer.h"
#include <stdio.h>

void trata_erros (Token * tok) {
    if(tok->type != TOKEN_ERROR){
        return;
    }

    printf("\nErro na linha %d, coluna %d", tok->line, tok->column);
}
