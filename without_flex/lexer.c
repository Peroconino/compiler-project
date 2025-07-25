#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "lexer.h"

#define BUFFER_SIZE 200

char buffer[BUFFER_SIZE + 1];
size_t buffer_pos = 0;
size_t buffer_len = 0;
int current_line = 1;
int current_column = 1;

// Função para recarregar o buffer quando necessário
int reload_buffer(FILE *file) {
  buffer_len = fread(buffer, sizeof(char), BUFFER_SIZE, file);
  buffer_pos = 0;
  buffer[buffer_len] = '\0';
  return buffer_len > 0;
}

Token *proximo_token(FILE *file) {

  static Token token;
  // valores default pra caso seja necessario verificar se existem valores
  // nesses campos
  token.operador = -1;
  token.pontuacao = -1;
  token.relop = -1;

  // Recarrega o buffer se necessário
  if (buffer_pos >= buffer_len) {
    if (!reload_buffer(file)) {
      token.type = TOKEN_EOF;
      return &token;
    }
  }

  // Pula espaços em branco
  while (isspace(buffer[buffer_pos])) {
    if (buffer[buffer_pos] == '\n') {
      current_line++;
      current_column = 1;
    } else {
      current_column++;
    }
    buffer_pos++;

    // Verifica se precisa recarregar o buffer
    if (buffer_pos >= buffer_len) {
      if (!reload_buffer(file)) {
        token.type = TOKEN_EOF;
        return &token;
      }
    }
  }

  // Início do token
  char *start = &buffer[buffer_pos];
  int start_column = current_column;
  // Comentario, palavras-chave e IDs
  if (isalpha(buffer[buffer_pos]) || buffer[buffer_pos] == '_') {
    // Verifica se é o início de um comentário
    if (buffer[buffer_pos] == '/' && buffer_pos + 1 < buffer_len &&
        buffer[buffer_pos + 1] == '*') {
      // Pula os dois caracteres do início do comentário
      buffer_pos += 2;
      current_column += 2;

      // Procura pelo fim do comentário
      int comment_closed = 0;
      while (!comment_closed) {
        if (buffer_pos >= buffer_len) {
          if (!reload_buffer(file)) {
            // Comentário não fechado até o final do arquivo
            token.type = TOKEN_ERROR;
            token.line = current_line;
            token.column = start_column;
            return &token;
          }
        }

        // Verifica se encontrou o fechamento */
        if (buffer[buffer_pos] == '*' && buffer_pos + 1 < buffer_len &&
            buffer[buffer_pos + 1] == '/') {
          buffer_pos += 2;
          current_column += 2;
          comment_closed = 1;
        } else {
          // Conta linhas dentro do comentário
          if (buffer[buffer_pos] == '\n') {
            current_line++;
            current_column = 1;
          } else {
            current_column++;
          }
          buffer_pos++;
        }
      }

      // Depois de ignorar o comentário, chama a função novamente
      return proximo_token(file);
    }

    // Processamento normal de identificador
    int length = 0;
    while (isalnum(buffer[buffer_pos]) || buffer[buffer_pos] == '_') {
      token.value[length++] = buffer[buffer_pos++];
      current_column++;

      if (buffer_pos >= buffer_len) {
        if (!reload_buffer(file))
          break;
      }
    }
    token.value[length] = '\0';

    // Verifica se é palavra-chave
    if (strcmp(token.value, "faca") == 0 ||
        strcmp(token.value, "enquanto") == 0 ||
        strcmp(token.value, "main") == 0 ||
        strcmp(token.value, "inicio") == 0 || strcmp(token.value, "fim") == 0 ||
        strcmp(token.value, "caso") == 0 || strcmp(token.value, "entao") == 0 ||
        strcmp(token.value, "senao") == 0 ||
        strcmp(token.value, "repita") == 0 || strcmp(token.value, "ate") == 0) {
      token.type = TOKEN_KEYWORD;
    } else {
      token.type = TOKEN_ID;
    }

    token.line = current_line;
    token.column = start_column;
    return &token;
  }
  // Operadores aritméticos
  else if (strchr("+-*/^()", buffer[buffer_pos])) {
    token.value[0] = buffer[buffer_pos++];
    current_column++;
    token.type = TOKEN_OPERATOR;

    switch (token.value[0]) {
    case '+':
      token.operador = SOMA;
      break;

    case '-':
      token.operador = SUB;
      break;

    case '*':
      token.operador = MULT;
      break;

    case '/':
      token.operador = DIV;
      break;
    case '^':
      token.operador = EXP;
      break;
    case '(':
      token.operador = PARESQ;
      break;
    case ')':
      token.operador = PARDIR;
      break;
    }

    token.line = current_line;
    token.column = start_column;
    return &token;
  }
  // Operadores relop e atribuição
  else if (strchr("<>=!", buffer[buffer_pos])) {
    token.value[0] = buffer[buffer_pos++];
    token.type = TOKEN_RELOP;
    // Verifica operadores de dois caracteres (==, !=, <=, >=, etc)
    if (buffer_pos < buffer_len && strchr("=<>", buffer[buffer_pos])) {
      token.value[1] = buffer[buffer_pos++];
      current_column++;
      token.value[2] = '\0';
    } else {
      token.value[1] = '\0';
    }

    switch (token.value[0]) {
    case '<':
      if (token.value[1] == '=') {
        token.relop = LE;
      } else {
        token.relop = LT;
      }
      break;

    case '>':
      if (token.value[1] == '=') {
        token.relop = GE;
      } else {
        token.relop = GT;
      }
      break;

    case '!':
      if (token.value[1] == '=') {
        token.relop = NE;
      } else {
        token.type = TOKEN_ERROR;
      }
      break;
    case '=':
      token.pontuacao = ASSIGMENT;
    }

    token.line = current_line;
    token.column = start_column;
    return &token;
    // Tokens de pontuação
  } else if (strchr("-;,", buffer[buffer_pos])) {
    token.value[0] = buffer[buffer_pos++];
    token.type = TOKEN_PUNCTUATION;
    // verifica se é o sinal de declaracao ->
    if (buffer_pos < buffer_len && strchr(">", buffer[buffer_pos])) {
      token.value[1] = buffer[buffer_pos++];
      current_column++;
      token.value[2] = '\0';
    } else {
      token.value[1] = '\0';
    }

    switch (token.value[0]) {
    case '-':
      if (token.value[1] == '>') {
        token.pontuacao = DECLARATION;
      }
      break;

    case ';':
      token.pontuacao = END_EXP;
      break;

    case ',':
      token.pontuacao = MUL_VARS;
      break;
    }

    token.line = current_line;
    token.column = start_column;
    return &token;
  }
  // Números
  else if (isdigit(buffer[buffer_pos])) {
    int length = 0;
    int has_fraction = 0;
    int has_exponent = 0;

    // Parte inteira (obrigatória)
    while (isdigit(buffer[buffer_pos])) {
      token.value[length++] = buffer[buffer_pos++];
      current_column++;

      if (buffer_pos >= buffer_len) {
        if (!reload_buffer(file))
          break;
      }
    }

    // Parte fracionária (opcional)
    if (buffer[buffer_pos] == '.') {
      has_fraction = 1;
      token.value[length++] = buffer[buffer_pos++];
      current_column++;

      if (buffer_pos >= buffer_len) {
        if (!reload_buffer(file)) {
          // Tratar erro: número terminou com ponto
          token.type = TOKEN_ERROR;
          return &token;
        }
      }

      // Dígitos após o ponto (pelo menos um é necessário)
      if (!isdigit(buffer[buffer_pos])) {
        // Tratar erro: faltam dígitos após o ponto
        token.type = TOKEN_ERROR;
        return &token;
      }

      while (isdigit(buffer[buffer_pos])) {
        token.value[length++] = buffer[buffer_pos++];
        current_column++;

        if (buffer_pos >= buffer_len) {
          if (!reload_buffer(file))
            break;
        }
      }
    }

    // Expoente (opcional)
    if (tolower(buffer[buffer_pos]) == 'e') {
      has_exponent = 1;
      token.value[length++] = buffer[buffer_pos++];
      current_column++;

      if (buffer_pos >= buffer_len) {
        if (!reload_buffer(file)) {
          // Tratar erro: número terminou com 'e'
          token.type = TOKEN_ERROR;
          return &token;
        }
      }

      // Sinal do expoente (opcional)
      if (buffer[buffer_pos] == '+' || buffer[buffer_pos] == '-') {
        token.value[length++] = buffer[buffer_pos++];
        current_column++;

        if (buffer_pos >= buffer_len) {
          if (!reload_buffer(file)) {
            // Tratar erro: número terminou após sinal do expoente
            token.type = TOKEN_ERROR;
            return &token;
          }
        }
      }

      // Dígitos do expoente (pelo menos um é necessário)
      if (!isdigit(buffer[buffer_pos])) {
        // Tratar erro: faltam dígitos no expoente
        token.type = TOKEN_ERROR;
        return &token;
      }

      while (isdigit(buffer[buffer_pos])) {
        token.value[length++] = buffer[buffer_pos++];
        current_column++;

        if (buffer_pos >= buffer_len) {
          if (!reload_buffer(file))
            break;
        }
      }
    }

    // Finaliza o token
    token.value[length] = '\0';
    token.type = TOKEN_NUMBER;
    token.line = current_line;
    token.column = start_column;
    return &token;
  }
  // Outros casos (adaptar conforme sua linguagem)
  // Token não reconhecido (pode tratar como erro ou caracter inválido)
  token.value[0] = buffer[buffer_pos++];
  token.value[1] = '\0';
  token.type = TOKEN_ERROR;
  token.line = current_line;
  token.column = current_column;
  current_column++;
  return &token;
}
