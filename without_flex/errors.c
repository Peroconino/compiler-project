#include "lexer.h"
#include <stdio.h>

void trata_erros(Token *tok) {
  if (tok->type != TOKEN_ERROR) {
    return;
  }

  printf("Erro na linha %d, coluna %d:", tok->line, tok->column);
  printf("\nToken.value: %s\n", tok->value);
  switch (tok->error) {
    case UNCLOSED_COMMENT:
      printf("Comentário nunca é fechado. Era esperado '*/'(fechamento de "
             "comentário).");
      break;

    case INVALID_TOKEN_AFTER_EXCLAMATION:
      printf("Token inesperado. Era esperado '='(igual) após.");
      break;

    case FRACTION_ENDED_WITH_A_DOT:
      printf("Parte fracionaria terminou com um ponto. Era esperado digitos "
             "decimais.");
      break;
    case ENDED_WITH_E_EXPOENT:
      printf("Expoente terminou com 'e'. Era esperado a continuação da notação "
             "científica. Exemplos: e12, E-7, e+42.");
      break;
    case ENDED_AFTER_EXPOENT_SIGN:
      printf("Exponte terminou com '+' ou '-'. Era esperado a continuação da "
             "notação científica. Exemplos: e12, E-7, e+42.");
      break;
      break;
    case UNKNOWN_TOKEN:
      printf("Token não reconhecido pela linguagem.");
      break;
  }
  printf("\n");
}
