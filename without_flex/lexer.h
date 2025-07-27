#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

typedef enum {
  TOKEN_ID,
  TOKEN_NUMBER,
  TOKEN_OPERATOR,
  TOKEN_PUNCTUATION,
  TOKEN_KEYWORD,
  TOKEN_EOF,
  TOKEN_ERROR,
  TOKEN_RELOP,
} TokenType;

typedef enum {
  GT,
  LT,
  LE,
  EQ,
  NE,
  GE,
} Relop;

typedef enum {
  SOMA,
  SUB,
  MULT,
  DIV,
  EXP,
  PARESQ,
  PARDIR,
} Operator;

typedef enum {
  CHAR,
  INT,
  FLOAT,
} Type;

typedef enum {
  ASSIGMENT,
  MUL_VARS,
  END_EXP,
  DECLARATION,
} Punctuation;

typedef enum {
  UNCLOSED_COMMENT,
  INVALID_TOKEN_AFTER_EXCLAMATION,
  FRACTION_ENDED_WITH_A_DOT,
  ENDED_WITH_E_EXPOENT,
  ENDED_AFTER_EXPOENT_SIGN,
  UNKNOWN_TOKEN,
} ErrorKind;

typedef struct {
  TokenType type;
  char value[50];
  Operator operador;
  Punctuation pontuacao;
  Relop relop;
  ErrorKind error;
  int line;
  int column;
} Token;

// Declaração da função do lexer
Token *proximo_token(FILE *file);
int reload_buffer(FILE *file);
#endif
