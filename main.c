#include "lexer.h"
#include <stddef.h>
#include <stdio.h>

int main() {
  FILE *input = fopen("entrada.txt", "r");
  if (!input) {
    printf("Erro ao abrir o arquivo");
    return 1;
  }

  init_state(input); // inicializa estado do lexer

  Token *token; // ponteiro pro token alocado

  printf("Análise Léxica:\n");
  printf("----------------\n");

  printf("<TOKEN_TYPE, LEXEME>\n");
  do {
    token = getNextToken();
    printf("<%d, %s>\n", token->type, token->value);
  } while (token->type != TOKEN_EOF && token->type != TOKEN_ERROR);
  fclose(input);
  return 0;
}
