#include "errors.h"
#include "lexer.h"
#include "symbol_table.h"
#include <stdio.h>
#include <stddef.h>

#define ARQUIVO "entrada.txt"

int main() {
    FILE* file = fopen(ARQUIVO, "r");
    if (!file) {
        printf("Erro ao abrir arquivo\n");
        return 1;
    }

    init_symbol_table();
    reload_buffer(file);

    Token* token = proximo_token(file);
    while (token->type != TOKEN_EOF) {
        if (token->type == TOKEN_ERROR) {
            trata_erros(token);
            break;
        }
        // Insere apenas se for ID ou NUMBER
        if (token->type == TOKEN_ID || token->type == TOKEN_NUMBER) {
            if (lookup_symbol(token->value) == NULL) {
                insert_symbol(token->value, token->type, token->line, token->column);
            }
        }
        printf("Token: %d, Valor: '%s', Linha: %d, Coluna: %d\n",
               token->type, token->value, token->line, token->column);
        token = proximo_token(file);
    }

    print_symbol_table();
    fclose(file);
    return 0;
}
