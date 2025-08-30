#ifndef LEXER_H
#define LEXER_H
#include <stdio.h>
#define ARQUIVO "entrada.txt"
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

typedef enum {
  GT,
  LT,
  LE,
  EQ,
  NE,
  GE,
} Relop;

typedef enum {
  SUM,
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
  APOSTROPHE,
  DOT,
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
} Keyword;

typedef enum {
  UNDEFINED = -1,
  UNCLOSED_COMMENT,
  INVALID_TOKEN_AFTER_EXCLAMATION,
  FRACTION_ENDED_WITH_A_DOT,
  ENDED_WITH_E_EXPOENT,
  ENDED_AFTER_EXPOENT_SIGN,
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

Token *proximo_token(FILE *file);
int reload_buffer(FILE *file);
int estado_inicial();
int final(int state);
void load_buffer(char *buffer);
void init_state(FILE *file);
char prox_char();
char peek_char();
Keyword get_keyword_type(const char *lexeme);
Token *acoes(int s);
int getCharClass(char c);
int move(int state, char c);
void ignore_spaces_and_comentaries();
Token *getNextToken();

#include "ast.h" 

struct ASTNode* procedimento_programa();
struct ASTNode* procedimento_bloco();
struct ASTNode* procedimento_decl_vars();
struct ASTNode* procedimento_lista_decl();
struct ASTNode* procedimento_decl_var();
struct ASTNode* procedimento_lista_ids();
struct ASTNode* procedimento_tipo();
struct ASTNode* procedimento_seq_comandos();



#endif
