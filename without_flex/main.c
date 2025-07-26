#include "errors.h"
#include "lexer.h"
#include "stdio.h"
#include <stddef.h>
#define ARQUIVO "entrada.txt"
#define TAMANHO 200

int main() {
  FILE *file = fopen(ARQUIVO, "r");
  if (!file) {
    printf("Erro ao abrir arquivo");
    return 1;
  }

  // Carrega o buffer inicial
  reload_buffer(file);

  Token *token;
  while (token->type != TOKEN_EOF) {
    token = proximo_token(file);
    printf("Token: %d, Valor: '%s', Linha: %d, Coluna: %d\n", token->type,
           token->value, token->line, token->column);
    trata_erros(token);
  }

  fclose(file);
  return 0;
}
