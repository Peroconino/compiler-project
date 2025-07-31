#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "lexer.h"

char buffer[BUFFER_SIZE + 1];
size_t buffer_pos = 0;
size_t buffer_len = 0;
int current_line = 1;
int current_column = 1;

int reload_buffer(FILE *file) {
  buffer_len = fread(buffer, sizeof(char), BUFFER_SIZE, file);
  buffer_pos = 0;
  buffer[buffer_len] = '\0';
  return buffer_len > 0;
}

int is_custom_keyword(const char* str) {
    const char* keywords[] = {
        "faca", "enquanto", "main", "inicio", "fim",
        "caso", "entao", "senao", "repita", "ate",
        "int", "float", "char", "if", "else",
        "while", "for", "return"
    };
    int n = sizeof(keywords)/sizeof(keywords[0]);
    for (int i=0;i<n;i++)
        if (strcmp(str, keywords[i]) == 0)
            return 1;
    return 0;
}

Token *proximo_token(FILE *file) {
  static Token token;
  token.operador = -1;
  token.pontuacao = -1;
  token.relop = -1;
  token.error = -1;

  if (buffer_pos >= buffer_len) {
    if (!reload_buffer(file)) {
      token.type = TOKEN_EOF;
      return &token;
    }
  }

  while (isspace(buffer[buffer_pos])) {
    if (buffer[buffer_pos] == '\n') {
      current_line++;
      current_column = 1;
    } else {
      current_column++;
    }
    buffer_pos++;
    if (buffer_pos >= buffer_len) {
      if (!reload_buffer(file)) {
        token.type = TOKEN_EOF;
        return &token;
      }
    }
  }

  int start_column = current_column;

  // Comentário tipo /* ... */
  if (buffer[buffer_pos] == '/' && buffer_pos + 1 < buffer_len &&
      buffer[buffer_pos + 1] == '*') {
    buffer_pos += 2;
    current_column += 2;
    int comment_closed = 0;
    while (!comment_closed) {
      if (buffer_pos >= buffer_len) {
        if (!reload_buffer(file)) {
          token.type = TOKEN_ERROR;
          token.error = UNCLOSED_COMMENT;
          token.line = current_line;
          token.column = start_column;
          token.value[0] = '\0';
          return &token;
        }
      }
      if (buffer[buffer_pos] == '*' && buffer_pos + 1 < buffer_len &&
          buffer[buffer_pos + 1] == '/') {
        buffer_pos += 2;
        current_column += 2;
        comment_closed = 1;
      } else {
        if (buffer[buffer_pos] == '\n') {
          current_line++;
          current_column = 1;
        } else {
          current_column++;
        }
        buffer_pos++;
      }
    }
    return proximo_token(file);
  }

  // Identificadores e palavras-chave
  if (isalpha(buffer[buffer_pos]) || buffer[buffer_pos] == '_') {
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
    token.type = is_custom_keyword(token.value) ? TOKEN_KEYWORD : TOKEN_ID;
    token.line = current_line;
    token.column = start_column;
    return &token;
  }

  // Pontuação
  if (strchr("-;,", buffer[buffer_pos])) {
    token.value[0] = buffer[buffer_pos++];
    token.type = TOKEN_PUNCTUATION;
    if (buffer_pos < buffer_len && strchr(">", buffer[buffer_pos])) {
      token.value[1] = buffer[buffer_pos++];
      current_column++;
      token.value[2] = '\0';
    } else {
      token.value[1] = '\0';
    }
    switch (token.value[0]) {
      case '-':
        if (token.value[1] == '>') token.pontuacao = DECLARATION;
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

  // Operadores aritméticos
  if (strchr("+-*/^()", buffer[buffer_pos])) {
    token.value[0] = buffer[buffer_pos++];
    token.value[1] = '\0';
    current_column++;
    token.type = TOKEN_OPERATOR;
    switch (token.value[0]) {
      case '+': token.operador = SOMA; break;
      case '-': token.operador = SUB; break;
      case '*': token.operador = MULT; break;
      case '/': token.operador = DIV; break;
      case '^': token.operador = EXP; break;
      case '(': token.operador = PARESQ; break;
      case ')': token.operador = PARDIR; break;
    }
    token.line = current_line;
    token.column = start_column;
    return &token;
  }

  // Operadores relop e atribuição
  if (strchr("<>=!", buffer[buffer_pos])) {
    token.value[0] = buffer[buffer_pos++];
    token.type = TOKEN_RELOP;
    if (buffer_pos < buffer_len && strchr("=<>", buffer[buffer_pos])) {
      token.value[1] = buffer[buffer_pos++];
      current_column++;
      token.value[2] = '\0';
    } else {
      token.value[1] = '\0';
    }
    switch (token.value[0]) {
      case '<':
        token.relop = (token.value[1] == '=') ? LE : LT;
        break;
      case '>':
        token.relop = (token.value[1] == '=') ? GE : GT;
        break;
      case '!':
        if (token.value[1] == '=') token.relop = NE;
        else {
          token.type = TOKEN_ERROR;
          token.error = INVALID_TOKEN_AFTER_EXCLAMATION;
        }
        break;
      case '=':
        token.pontuacao = ASSIGMENT;
        break;
    }
    token.line = current_line;
    token.column = start_column;
    return &token;
  }

  // Números
  if (isdigit(buffer[buffer_pos])) {
    int length = 0;
    int has_fraction = 0;
    int has_exponent = 0;
    while (isdigit(buffer[buffer_pos])) {
      token.value[length++] = buffer[buffer_pos++];
      current_column++;
      if (buffer_pos >= buffer_len) {
        if (!reload_buffer(file)) break;
      }
    }
    // Parte fracionária (opcional)
    if (buffer[buffer_pos] == '.') {
      has_fraction = 1;
      token.value[length++] = buffer[buffer_pos++];
      current_column++;
      if (buffer_pos >= buffer_len) {
        if (!reload_buffer(file)) {
          token.type = TOKEN_ERROR;
          token.error = FRACTION_ENDED_WITH_A_DOT;
          token.line = current_line;
          token.column = start_column;
          token.value[length] = '\0';
          return &token;
        }
      }
      if (!isdigit(buffer[buffer_pos])) {
        token.type = TOKEN_ERROR;
        token.error = FRACTION_ENDED_WITH_A_DOT;
        token.line = current_line;
        token.column = start_column;
        token.value[length] = '\0';
        return &token;
      }
      while (isdigit(buffer[buffer_pos])) {
        token.value[length++] = buffer[buffer_pos++];
        current_column++;
        if (buffer_pos >= buffer_len) {
          if (!reload_buffer(file)) break;
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
          token.type = TOKEN_ERROR;
          token.error = ENDED_WITH_E_EXPOENT;
          token.line = current_line;
          token.column = start_column;
          token.value[length] = '\0';
          return &token;
        }
      }
      if (buffer[buffer_pos] == '+' || buffer[buffer_pos] == '-') {
        token.value[length++] = buffer[buffer_pos++];
        current_column++;
        if (buffer_pos >= buffer_len) {
          if (!reload_buffer(file)) {
            token.type = TOKEN_ERROR;
            token.error = ENDED_AFTER_EXPOENT_SIGN;
            token.line = current_line;
            token.column = start_column;
            token.value[length] = '\0';
            return &token;
          }
        }
      }
      if (!isdigit(buffer[buffer_pos])) {
        token.type = TOKEN_ERROR;
        token.error = ENDED_AFTER_EXPOENT_SIGN;
        token.line = current_line;
        token.column = start_column;
        token.value[length] = '\0';
        return &token;
      }
      while (isdigit(buffer[buffer_pos])) {
        token.value[length++] = buffer[buffer_pos++];
        current_column++;
        if (buffer_pos >= buffer_len) {
          if (!reload_buffer(file)) break;
        }
      }
    }
    token.value[length] = '\0';
    token.type = TOKEN_NUMBER;
    token.line = current_line;
    token.column = start_column;
    return &token;
  }

  // Token desconhecido
  token.value[0] = buffer[buffer_pos++];
  token.value[1] = '\0';
  token.type = TOKEN_ERROR;
  token.error = UNKNOWN_TOKEN;
  current_column++;
  token.line = current_line;
  token.column = current_column;
  return &token;
}
