#include "lexer.h"
#include "symbol_table.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Inicializa a variável global do estado do lexer
LexerState state;

// Tabela de palavras-chave
const KeywordEntry keyword_table[] = {
    {"caso", IF},       {"entao", THEN},    {"senao", ELSE},
    {"enquanto", WHILE},  {"faca", DO},      {"repita", REPEAT},
    {"ate", UNTIL},     {"main", MAIN},     {"inicio", BEGIN},
    {"fim", END},       {"int", TYPE_INT},   {"char", TYPE_CHAR},
    {"float", TYPE_FLOAT}, {NULL, 0}};

// --- Funções Auxiliares ---

// Inicializa o estado do lexer e carrega o arquivo na memória
void init_lexer(FILE *file) {
  fseek(file, 0, SEEK_END);
  state.buffer_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  state.source_buffer = (char *)malloc(state.buffer_size + 1);
  fread(state.source_buffer, 1, state.buffer_size, file);
  state.source_buffer[state.buffer_size] = EOF;

  state.file = file;
  state.current_pos = 0;
  state.line = 1;
  state.column = 1;
}

// Retorna o caractere atual e avança o ponteiro
static char advance() {
  if (state.current_pos >= state.buffer_size) {
    return EOF;
  }
  char c = state.source_buffer[state.current_pos++];
  if (c == '\n') {
    state.line++;
    state.column = 1;
  } else {
    state.column++;
  }
  return c;
}

// Retorna o caractere atual sem avançar (lookahead)
static char peek() {
  if (state.current_pos >= state.buffer_size) {
    return EOF;
  }
  return state.source_buffer[state.current_pos];
}

// Retorna o próximo caractere sem avançar
static char peek_next() {
  if (state.current_pos + 1 >= state.buffer_size) {
    return EOF;
  }
  return state.source_buffer[state.current_pos + 1];
}

// Cria um novo token
static Token *make_token(TokenType type) {
  Token *token = (Token *)malloc(sizeof(Token));
  token->type = type;
  token->line = state.line;
  
  int current_line_start = state.token_start_pos;
  while(current_line_start > 0 && state.source_buffer[current_line_start-1] != '\n') {
      current_line_start--;
  }
  token->column = state.token_start_pos - current_line_start + 1;


  int length = state.current_pos - state.token_start_pos;
  if (length >= MAX_LEXEME_LENGTH) {
      length = MAX_LEXEME_LENGTH - 1;
  }
  strncpy(token->value, &state.source_buffer[state.token_start_pos], length);
  token->value[length] = '\0';
  return token;
}


// Ignora espaços em branco e comentários
static void skip_whitespace_and_comments() {
  while (1) {
    char c = peek();
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
      advance();
    } else if (c == '/' && peek_next() == '*') {
      advance(); 
      advance(); 
      while (!(peek() == '*' && peek_next() == '/') && peek() != EOF) {
        advance();
      }
      if (peek() != EOF) {
        advance(); 
        advance(); 
      }
    } else {
      break;
    }
  }
}

// Processa um número (inteiro ou float)
static Token *make_number() {
  bool is_float = false;
  while (isdigit(peek())) {
    advance();
  }

  if (peek() == '.' && isdigit(peek_next())) {
    is_float = true;
    advance(); 
    while (isdigit(peek())) {
      advance();
    }
  }
  
  if (toupper(peek()) == 'E' && (peek_next() == '+' || peek_next() == '-' || isdigit(peek_next()))) {
      is_float = true;
      advance(); 
      if (peek() == '+' || peek() == '-') {
          advance();
      }
      while(isdigit(peek())) {
          advance();
      }
  }

  Token *token = make_token(TOKEN_NUMBER);
  token->numberType = is_float ? FLOAT : INT;

  if (lookup_symbol(token->value) == NULL) {
    insert_symbol(token->value, TOKEN_NUMBER, token->line, token->column);
  }

  return token;
}

// Processa um identificador ou palavra-chave
static Token *make_identifier() {
  while (isalnum(peek()) || peek() == '_') {
    advance();
  }

  Token *token = make_token(TOKEN_ID);

  for (int i = 0; keyword_table[i].value != NULL; i++) {
    if (strcmp(token->value, keyword_table[i].value) == 0) {
      token->type = TOKEN_KEYWORD;
      token->keyword = keyword_table[i].keyword;
      return token;
    }
  }

  if (lookup_symbol(token->value) == NULL) {
    insert_symbol(token->value, TOKEN_ID, token->line, token->column);
  }

  return token;
}

// Função Principal do Analisador Léxico
Token *getNextToken() {
  skip_whitespace_and_comments();

  state.token_start_pos = state.current_pos;
  state.token_start_col = state.column;

  char c = advance();

  if (c == EOF) return make_token(TOKEN_EOF);
  if (isalpha(c) || c == '_') return make_identifier();
  if (isdigit(c)) return make_number();

  Token* token;

  switch (c) {
    case '(': token = make_token(TOKEN_OPERATOR); token->operador = PARESQ; return token;
    case ')': token = make_token(TOKEN_OPERATOR); token->operador = PARDIR; return token;
    case '+': token = make_token(TOKEN_OPERATOR); token->operador = SUM; return token;
    case '*': token = make_token(TOKEN_OPERATOR); token->operador = MULT; return token;
    case '/': token = make_token(TOKEN_OPERATOR); token->operador = DIV; return token;
    case '^': token = make_token(TOKEN_OPERATOR); token->operador = EXP; return token;
    
    case ';': token = make_token(TOKEN_PUNCTUATION); token->pontuacao = END_EXP; return token;
    case ',': token = make_token(TOKEN_PUNCTUATION); token->pontuacao = MUL_VARS; return token;
    
    // CORREÇÃO: Trata o literal de char corretamente
    case '\'': {
        state.token_start_pos = state.current_pos; // Marca o início do conteúdo do char
        char char_val = advance(); // Lê o caractere
        if (peek() == '\'') {
            advance(); // Consome o apóstrofo de fechamento
            // Cria um token do tipo número, subtipo char
            Token* char_token = make_token(TOKEN_NUMBER);
            char_token->numberType = CHAR;
            char_token->value[0] = char_val; // Armazena o caractere diretamente
            char_token->value[1] = '\0';
            if (lookup_symbol(char_token->value) == NULL) {
                insert_symbol(char_token->value, TOKEN_NUMBER, char_token->line, char_token->column);
                set_symbol_type(char_token->value, CHAR); // Define o tipo como CHAR
            }
            return char_token;
        } else {
            // Erro: literal de char com múltiplos caracteres ou não fechado
            Token *err_token = make_token(TOKEN_ERROR);
            err_token->error = UNKNOWN_TOKEN; // Simplificado como token desconhecido
            while (peek() != '\'' && peek() != '\n' && peek() != EOF) advance();
            if (peek() == '\'') advance();
            printf("Erro Lexico (Linha %d, Coluna %d): Literal de char invalido\n", state.line, state.column);
            return err_token;
        }
    }
    
    case '-':
        if (peek() == '>') {
            advance();
            token = make_token(TOKEN_PUNCTUATION);
            token->pontuacao = DECLARATION;
        } else {
            token = make_token(TOKEN_OPERATOR);
            token->operador = SUB;
        }
        return token;
    
    case '=':
        if (peek() == '=') {
            advance();
            token = make_token(TOKEN_RELOP);
            token->relop = EQ;
        } else {
            token = make_token(TOKEN_OPERATOR);
            token->operador = ASSIGN;
        }
        return token;
    
    case '>':
        token = make_token(TOKEN_RELOP);
        if (peek() == '=') {
            advance();
            token->relop = GE;
        } else {
            token->relop = GT;
        }
        return token;

    case '<':
        token = make_token(TOKEN_RELOP);
        if (peek() == '=') {
            advance();
            token->relop = LE;
        } else {
            token->relop = LT;
        }
        return token;

    case '!':
        if (peek() == '=') {
            advance();
            token = make_token(TOKEN_RELOP);
            token->relop = NE;
            return token;
        }
        break; 
  }

  // Se chegou aqui, é um token desconhecido
  Token *err_token = make_token(TOKEN_ERROR);
  err_token->error = UNKNOWN_TOKEN;
  return err_token;
}