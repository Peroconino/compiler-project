#include "lexer.h"
#include "stdio.h"
#include <stddef.h>
#define ARQUIVO "entrada.txt"
#define TAMANHO 200

int main() {
  FILE *file = fopen("entrada.txt", "r");
  if (!file) {
    perror("Erro ao abrir arquivo");
    return 1;
  }

  // Carrega o buffer inicial
  reload_buffer(file);

  Token *token;
  while ((token = proximo_token(file))->type != TOKEN_EOF) {
    printf("Token: %d, Valor: '%s', Linha: %d, Coluna: %d\n", token->type,
           token->value, token->line, token->column);
  }

  fclose(file);
  return 0;
}
