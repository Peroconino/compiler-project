#include "lexer.h"
#include <stddef.h>
#include <stdio.h>

void imprime_token_atual(Token *proxToken) {
  printf("\nFoi encontrado '%s' em vez disso", proxToken->value);
}

void procedimento_lista_ids() {
  Token *proxToken = getNextToken();
  if (proxToken->type == TOKEN_ID) {
    // prodecimento_cont_lista_ids()
    Token *proxToken = getNextToken();
    if (proxToken->type == TOKEN_PUNCTUATION &&
        proxToken->pontuacao == MUL_VARS) {
      procedimento_lista_ids();
    }
    // fim cont_list_ids()
  }
  return;
}

void procedimento_tipo() {
  Token *proxToken = getNextToken();
  if (proxToken->type == TOKEN_NUMBER &&
      (proxToken->numberType == INT || proxToken->numberType == CHAR ||
       proxToken->numberType == FLOAT)) {
    return;
  }
}

void procedimento_decl_var() {
  procedimento_lista_ids();

  Token *proxToken = getNextToken();
  if (proxToken->type == TOKEN_PUNCTUATION &&
      proxToken->pontuacao == ASSIGMENT) {
    procedimento_tipo();

    Token *proxToken = getNextToken();
    if (proxToken->type == TOKEN_PUNCTUATION &&
        proxToken->pontuacao == END_EXP) {
      return;
    } else {
      printf("Erro: era esperado ;");
    }
  } else {
    printf("Erro: era esperado um tipo");
  }

  imprime_token_atual(proxToken);
}

void procedimento_lista_decl(Token *proxToken) {

  while (proxToken->type == TOKEN_ID) {
    procedimento_decl_var();
    proxToken = getNextToken();
  }
}

void procedimento_decl_vars() {

  Token *proxToken = getNextToken();

  if (proxToken->type == TOKEN_ID) {
    procedimento_lista_decl(proxToken);
  }

  return;
}

void procedimento_bloco() {
  Token *proxToken = getNextToken();

  if (proxToken->type == TOKEN_KEYWORD && proxToken->keyword == BEGIN) {
    procedimento_decl_vars();
    procedimento_seq_comandos();

    Token *proxToken = getNextToken();
    if (proxToken->type == TOKEN_KEYWORD && proxToken->keyword == END) {
      return;
    } else {
      printf("Erro: Era esperado fim de bloco");
    }
  } else {
    printf("Erro: Era esperado inicio de bloco");
  }

  imprime_token_atual(proxToken);
}

void procedimento_programa() {
  Token *proxToken = getNextToken();

  if (proxToken->type == TOKEN_KEYWORD && proxToken->keyword == MAIN) {
    Token *proxToken = getNextToken();

    if (proxToken->type == TOKEN_ID) {
      Token *proxToken = getNextToken();

      if (proxToken->type == TOKEN_OPERATOR && proxToken->operador == PARESQ) {
        Token *proxToken = getNextToken();

        if (proxToken->type == TOKEN_OPERATOR &&
            proxToken->operador == PARDIR) {
          procedimento_bloco();
          return;
        } else {
          printf("Erro: Era esperado ) parentesis direito");
        }
      } else {
        printf("Erro: Era esperado ( parentesis esquerdo)");
      }
    } else {
      printf("Erro: Era esperado o nome do programa");
    }
  } else {
    printf("Erro: Era esperado main para o inicio de programa");
  }
  imprime_token_atual(proxToken);
}
