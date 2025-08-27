#include "lexer.h"
#include "symbol_table.h"
#include "ast.h" // Incluir o cabeçalho da AST
#include <stdio.h>

// Protótipo da função do parser
ASTNode* procedimento_programa();

// Protótipo da função para imprimir a AST
void print_ast(ASTNode* node, int level);

// Declaração da variável global de erro
extern bool hasError;

int main() {
  FILE *input = fopen("entrada.txt", "r");
  if (!input) {
    printf("Erro ao abrir o arquivo de entrada.\n");
    return 1;
  }

  // Inicializa o lexer e a tabela de símbolos
  init_lexer(input);
  init_symbol_table();

  printf("Iniciando Analise Sintatica...\n");
  printf("--------------------------------\n");

  // Chama o procedimento inicial do parser
  ASTNode* ast = procedimento_programa();
  
  if (!hasError) {
    printf("\nAnalise Sintatica concluida com sucesso!\n");
    printf("\nArvore Sintatica:\n");
    print_ast(ast, 0);
  }
  
  // Libera a memória
  free_ast(ast);
  
  // Imprime a tabela de símbolos no final
  print_symbol_table();

  fclose(input);
  return 0;
}