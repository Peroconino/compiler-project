#ifndef LEXER_H
#define LEXER_H
#include <stdio.h>
#define MAX_LEXEME_LENGTH 50
#define BUFFER_SIZE 4096

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

typedef enum { GT, LT, LE, EQ, NE, GE } Relop;

typedef enum {
  SUM,      // +
  SUB,      // -
  MULT,     // *
  DIV,      // /
  EXP,      // ^
  PARESQ,   // (
  PARDIR,   // )
  ASSIGN,   // =
} Operator;

typedef enum { CHAR, INT, FLOAT, UNDEFINED } Type;

typedef enum {
  MUL_VARS,    // ,
  END_EXP,     // ;
  DECLARATION, // ->
  APOSTROPHE,  // '
} Punctuation;

typedef enum {
  IF,
  THEN,
  ELSE,
  WHILE,
  DO,
  MAIN,
  BEGIN,
  END,
  REPEAT,
  UNTIL,
  // Adicionadas as palavras-chave de tipo
  TYPE_INT,
  TYPE_CHAR,
  TYPE_FLOAT
} Keyword;

typedef enum {
  ERROR_UNDEFINED = -1,
  UNCLOSED_COMMENT,
  UNKNOWN_TOKEN,
} ErrorKind;

typedef struct {
  TokenType type;
  char value[MAX_LEXEME_LENGTH];
  union {
    Operator operador;
    Keyword keyword;
    Type numberType;
    Punctuation pontuacao;
    Relop relop;
    ErrorKind error;
  };
  int line;
  int column;
} Token;

typedef struct {
  const char *value;
  Keyword keyword;
} KeywordEntry;

typedef struct {
  FILE *file;
  char *source_buffer;
  int buffer_size;
  int current_pos;
  int line;
  int column;
  int token_start_pos;
  int token_start_col;
} LexerState;

extern LexerState state;

void init_lexer(FILE *file);
Token *getNextToken();

#endif