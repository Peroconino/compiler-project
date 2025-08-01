#include "lexer.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE], *current_buffer, lookahead;
  int ini, prox, line, column;
  bool eof, has_lookahead;
  FILE *file;
} State;

// globals
State state;

// Tabela de transições para o autômato
const int transitionTable[35][20] = {
    // letra_, digito, ., =, >, <, +, -, *, /, ^, (, ), ,, ;,
    // !, ', EOF, ws\n\t, ->
    {16, 18, 31, 2, 3, 6, 11, 12, 13, 14, 15, 28, 27, 30, 29, 9, 32, 33,
     34}, // Estado inicial 1
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // Estado 2 '=': RELOP, EQ
    {5, 5, 5, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5}, // Estado 3 '>?':
    {-1, -1, -1, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // Estado 4 '>=': RELOP, GE
    {-1, -1, -1, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // Estado 5 '>': RELOP, GT
    {8, 8, 8, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8}, // Estado 6 '<?':
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // Estado 7 '<=': RELOP, LE
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // Estado 8 '<': RELOP, LT
    {-1, -1, -1, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // Estado 9 '!?':
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // Estado 10 '!=': RELOP, NE
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     34}, // Estado 11 '+': TOKEN_OPERATOR, SUM
    {-1, -1, -1, -1, 34, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     34}, // Estado 12 '-': TOKEN_OPERATOR, SUB
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     34}, // Estado 13 '*': TOKEN_OPERATOR, MUL
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     34}, // Estado 14 '/': TOKEN_OPERATOR, DIV
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     34}, // Estado 15 '^': TOKEN_OPERATOR, EXP
    {16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
     17}, // Estado 16 'letra_ + digito':
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // Estado 17 'letra_ + digito': TOKEN_ID
    {19, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
     19}, // Estado 18 'digito + ?':
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // Estado 19 'digito': TOKEN_NUMBER,
          // INT
    {-1, 21, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // Estado 20 'digito.?':
    {23, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
     22}, // Estado 21 'digito.digito?':
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // Estado 22 'digito.digito':
          // TOKEN_NUMBER, FLOAT
    {-1, -1, -1, -1, -1, -1, 24, 24, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // Estado 23 'digito.digitoE[+-]?':
    {-1, 24, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // Estado 24
          // 'digito.digitoE[+-]digito':
    {26, 24, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
     26}, // Estado 25
          // 'digito.digitoE[+-]digito':
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // Estado 26
          // 'digito.digitoE[+-]digito':
          // TOKEN_NUMBER, FLOAT
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // Estado 27 ')': TOKEN_OPERATOR,
          // PARDIR
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // Estado 28 '(': TOKEN_OPERATOR,
          // PARESQ
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // Estado 29 ';': TOKEN_PONTUACTION,
          // END_EXP
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // Estado 30 ',': TOKEN_PONTUACTION,
          // MUL_VARS
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // Estado 31'.': TOKEN_PONTUACTION,
          // DOT
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // estado 32'': TOKEN_EOF, 
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // estado 33: TOKEN_PONTUACTION, APOSTROPHE
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1}, // estado 34' \t\n': TOKEN_WHITESPACE
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 34,
     -1}, // estado 35'->': TOKEN_OPERATOR,
          // ASSIGMENT
};

int estado_inicial() { return 1; }

int final(int state) {
  return (state == -1 || state == 2 || state == 4 || state == 5 || state == 7 ||
          state == 8 || state == 10 || state == 11 || state == 13 ||
          state == 14 || state == 15 || state == 17 || state == 19 ||
          state == 22 || state == 24 || state == 26 || state == 27 ||
          state == 28 || state == 29 || state == 30 || state == 31 ||
          state == 32 || state == 33 || state == 34);
}

void load_buffer(char *buffer) {
  int buffer_len = fread(buffer, sizeof(char), BUFFER_SIZE, state.file);
  if (buffer_len < BUFFER_SIZE) {
    buffer[buffer_len] = EOF;
    if (buffer_len == 0)
      state.eof = true;
  }
}

void init_state(FILE *file) {
  state.file = file;
  state.line = 1;
  state.column = 1;
  state.ini = 0;
  state.prox = 0;
  state.eof = false;
  state.has_lookahead = false;
  load_buffer(state.buffer1);
  load_buffer(state.buffer2);
  state.current_buffer = state.buffer1;
}

char prox_char() {
  if (state.has_lookahead) {
    state.has_lookahead = false;
    return state.lookahead;
  }

  // Verifica se precisa trocar de buffer
  if (state.prox >= BUFFER_SIZE) {
    if (state.current_buffer == state.buffer1) {
      state.current_buffer = state.buffer2;
      // Carrega o próximo bloco no buffer1 (assíncrono seria ideal)
      if (!state.eof)
        load_buffer(state.buffer1);
    } else {
      state.current_buffer = state.buffer1;
      // Carrega o próximo bloco no buffer2
      if (!state.eof)
        load_buffer(state.buffer2);
    }
    state.prox = 0;
  }

  char c = state.current_buffer[state.prox];
  state.prox++;

  // Contagem de linhas
  if (c == '\n') {
    state.line++;
    state.column = 1;
  } else {
    state.column++;
  }

  return c;
}

char peek_char() {
  if (state.has_lookahead) {
    return state.lookahead;
  }

  state.lookahead = prox_char();
  state.has_lookahead = true;
  return state.lookahead;
}

const KeywordEntry keyword_table[] = {
    {"caso", IF},      {"entao", THEN},    {"senao", ELSE}, {"enquanto", WHILE},
    {"faca", DO},      {"repita", REPEAT}, {"ate", UNTIL},  {"main", MAIN},
    {"inicio", BEGIN}, {"fim", END},       {NULL, -1},
};

Keyword get_keyword_type(const char *lexeme) {
  for (int i = 0; keyword_table[i].value != NULL; i++) {
    if (strcmp(lexeme, keyword_table[i].value) == 0) {
      return keyword_table[i].keyword;
    }
  }

  return -1;
}

// Determina o tipo de token baseado no estado final
Token *acoes(int s) {
  Token *token = malloc(sizeof(Token));
  // Inicializa union enums com -1, indicando que não foi preenchido
  token->error = UNDEFINED;
  int lexeme_len = state.prox - state.ini;
  strncpy(token->value, &state.current_buffer[state.ini], lexeme_len);
  token->value[lexeme_len] = '\0';

  switch (s) {
  case 2:
    token->type = TOKEN_RELOP;
    token->relop = EQ;
    break;
  case 4:
    token->type = TOKEN_RELOP;
    token->relop = GE;
    break;
  case 5:
    if (token->value[1] != '=') { // lookahead
      token->type = TOKEN_RELOP;
      token->relop = GT;
      state.prox--;
      state.column--;
      token->value[1] = '\0';
    }
    break;
  case 7:
    if (token->value[1] != '=') { // lookahead
      token->type = TOKEN_RELOP;
      token->relop = LE;
      state.prox--;
      state.column--;
      token->value[1] = '\0';
    }
    break;
  case 8:
    token->type = TOKEN_RELOP;
    token->relop = LT;
    break;
  case 10:
    if (token->value[1] != '=') { // lookahead
      token->type = TOKEN_RELOP;
      token->relop = NE;
      state.prox--;
      state.column--;
      token->value[1] = '\0';
    }
    break;
  case 11:
    token->type = TOKEN_OPERATOR;
    token->operador = SUM;
    break;
  case 12:
    if (token->value[1] == '>') {
      token->type = TOKEN_PUNCTUATION;
      token->pontuacao = ASSIGMENT;
    } else {
      token->type = TOKEN_OPERATOR;
      token->operador = SUB;
    }
    break;
  case 13:
    token->type = TOKEN_OPERATOR;
    token->operador = MULT;
    break;
  case 14:
    token->type = TOKEN_OPERATOR;
    token->operador = DIV;
    break;
  case 15:
    token->type = TOKEN_OPERATOR;
    token->operador = EXP;
    break;
  case 17:
    if (!isdigit(token->value[lexeme_len - 1]) &&
        !isalpha(token->value[lexeme_len - 1])) {
      token->value[lexeme_len - 1] = '\0';
      state.prox--;
      state.column--;
    }
    token->type = TOKEN_ID;
    Keyword palavra_chave = get_keyword_type(token->value);

    if (palavra_chave != -1) {
      token->type = TOKEN_KEYWORD;
      token->keyword = palavra_chave;
      break;
    }

    // TODO  colocar na tabela de simbolos se nao existir
    break;
  case 19:
    token->type = TOKEN_NUMBER;
    token->numberType = INT;
    // TODO colocar na tabela de simbolos se nao existir
    break;
  case 22:
    token->type = TOKEN_NUMBER;
    token->numberType = FLOAT;
    // TODO colocar na tabela de simbolos se nao existir
    break;
  case 26: // com notacao cientifica
    token->type = TOKEN_NUMBER;
    token->numberType = FLOAT;
    // TODO colocar na tabela de simbolos se nao existir
    break;
  case 27:
    token->type = TOKEN_OPERATOR;
    token->operador = PARDIR;
    break;
  case 28:
    token->type = TOKEN_OPERATOR;
    token->operador = PARESQ;
    break;
  case 29:
    token->type = TOKEN_PUNCTUATION;
    token->pontuacao = END_EXP;
    break;
  case 30:
    token->type = TOKEN_PUNCTUATION;
    token->pontuacao = MUL_VARS;
    break;
  case 31:
    token->type = TOKEN_PUNCTUATION;
    token->pontuacao = DOT;
    break;
  case 32:
    token->type = TOKEN_PUNCTUATION;
    token->pontuacao = APOSTROPHE;
    break;
  case 33:
    token->type = TOKEN_EOF;
    break;
  case 34:
    token->type = TOKEN_PUNCTUATION;
    token->pontuacao = ASSIGMENT;
    break;
  default:
    token->type = TOKEN_ERROR;
    token->error = UNKNOWN_TOKEN;
  }

  if (token->type == TOKEN_NUMBER && !isdigit(token->value[lexeme_len - 1])) {
    token->value[lexeme_len - 1] = '\0';
    state.prox--;
    state.column--;
  }

  token->line = state.line;
  token->column = state.column;

  return token;
}

// Função para classificar um caractere
int getCharClass(char c) {
  if (isalpha(c) || c == '_')
    return 0; // Letra_
  if (isdigit(c))
    return 1; // Dígito
  // Relop
  if (strchr("><=!", c)) {
    if (strchr(">", c)) {
      return 4;
    } else if (strchr("<", c)) {
      return 5;
    } else if (strchr("=", c)) {
      return 3;
    } else if (strchr("!", c)) {
      return 15;
    }
  }
  // Operators
  if (strchr("+-*/^()", c)) {
    if (strchr("+", c)) {
      return 6;
    } else if (strchr("-", c)) {
      return 7;
    } else if (strchr("*", c)) {
      return 8;
    } else if (strchr("/", c)) {
      return 9;
    } else if (strchr("^", c)) {
      return 10;
    } else if (strchr("(", c)) {
      return 11;
    } else if (strchr(")", c)) {
      return 12;
    }
  }
  // Punctations
  if (strchr(".;,'", c)) {
    if (strchr(".", c)) {
      return 2;
    } else if (strchr(";", c)) {
      return 14;
    } else if (strchr(",", c)) {
      return 13;
    } else if (strchr("'", c)) {
      return 16;
    }
  }
  // whitespaces
  if (strchr(" \n\t", c)) {
    return 18;
  }

  if (c == EOF) {
    return 17;
  }

  return -1; // erro
}

int move(int state, char c) {
  int charClass = getCharClass(c);
  if (state < 1 || state > 36)
    return -1; // Estado invalido
  return transitionTable[state - 1][charClass];
}

void ignore_spaces_and_comentaries() {
  while (1) {
    char c = peek_char(); // Olha o próximo caractere sem consumir

    // Pula espaços, tabs e quebras de linha
    if (c == ' ' || c == '\t' || c == '\n') {
      prox_char();            // Consome o caractere
      state.ini = state.prox; // Atualiza o início do token
      if (c == '\n') {
        state.line++;
        state.column = 1;
      } else {
        state.column++;
      }
      continue;
    }

    // Trata comentários de bloco (/* ... */)
    if (c == '/' && peek_char() == '*') {
      prox_char();            // Consome '/'
      prox_char();            // Consome '*'
      state.ini = state.prox; // Atualiza o início do token

      while (1) {
        c = prox_char();
        if (c == EOF)
          break;
        if (c == '\n') {
          state.line++;
          state.column = 1;
        }
        if (c == '*' && peek_char() == '/') {
          prox_char();            // Consome '/'
          state.ini = state.prox; // Atualiza o início do token
          break;
        }
      }
      continue;
    }

    break; // Encontrou um caractere significativo
  }
}
// Função principal do analisador léxico
Token *getNextToken() {
  ignore_spaces_and_comentaries();

  int s = estado_inicial();
  char c;
  if (state.has_lookahead)
    state.ini = state.prox - 1;
  else
    state.ini = state.prox;

  while (!final(s)) {
    c = prox_char();
    s = move(s, c);
  }

  // Executar acoes dos estados finais ou tratar erros
  return acoes(s);
}
