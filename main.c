// #include "lexer.h"
// #include <stddef.h>
// #include <stdio.h>

// int main() {
//   FILE *input = fopen("entrada.txt", "r");
//   if (!input) {
//     printf("Erro ao abrir o arquivo");
//     return 1;
//   }

//   init_state(input); // inicializa estado do lexer

//   Token *token; // ponteiro pro token alocado

//   printf("Análise Léxica:\n");
//   printf("----------------\n");

//   printf("<TOKEN_TYPE, LEXEME>\n");
//   do {
//     token = getNextToken();
//     printf("<%d, %s>\n", token->type, token->value);
//   } while (token->type != TOKEN_EOF && token->type != TOKEN_ERROR);
//   fclose(input);
//   return 0;
// }

#include "lexer.h"
#include <stddef.h>
#include <stdio.h>

int main() {
  FILE *input = fopen("entrada.txt", "r");
  if (!input) {
    printf("Erro ao abrir o arquivo de entrada.\n");
    return 1;
  }

  // Inicializa o estado do analisador léxico
  init_state(input);

  printf("Iniciando Análise Sintática...\n");
  printf("-----------------------------\n");
  
  // Chama a regra inicial da gramática para construir a AST
  ASTNode* ast_root = procedimento_programa();

  if (ast_root) {
      printf("\nAnálise Sintática concluída com sucesso!\n");
      printf("Imprimindo a Árvore Sintática Abstrata (AST):\n");
      printf("---------------------------------------------\n");
      print_ast(ast_root, 0);
      
      // Libera a memória alocada para a AST
      free_ast(ast_root);
  } else {
      printf("\nAnálise Sintática falhou.\n");
  }

  fclose(input);
  return 0;
}