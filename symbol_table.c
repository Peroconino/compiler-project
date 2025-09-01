#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

SymbolTable symbol_table;

void init_symbol_table() { symbol_table.size = 0; }

SymbolEntry *insert_symbol(const char *lexeme, TokenType type, int line,
                           int column) {
  if (symbol_table.size >= MAX_SYMBOLS) {
    printf("Erro: Tabela de símbolos cheia\n");
    return NULL;
  }
  SymbolEntry *entry = &symbol_table.entries[symbol_table.size++];
  strncpy(entry->lexeme, lexeme, MAX_LEXEME_LENGTH - 1);
  entry->lexeme[MAX_LEXEME_LENGTH - 1] = '\0';
  entry->token_type = type;
  entry->has_value = false;
  entry->line = line;
  entry->column = column;
  entry->data_type = UNDEFINED;

  if (type == TOKEN_NUMBER) {
    if (strchr(lexeme, '.') || strchr(lexeme, 'e') || strchr(lexeme, 'E')) {
      entry->value.float_value = atof(lexeme);
      entry->data_type = FLOAT;
    } else if (strlen(lexeme) == 1 && isprint(lexeme[0]) && !isdigit(lexeme[0])) {
      entry->value.char_value = lexeme[0];
      entry->data_type = CHAR;
    } else {
      entry->value.int_value = atoi(lexeme);
      entry->data_type = INT;
    }
    entry->has_value = true;
  }
  return entry;
}

SymbolEntry *lookup_symbol(const char *lexeme) {
  for (int i = 0; i < symbol_table.size; i++) {
    if (strcmp(symbol_table.entries[i].lexeme, lexeme) == 0) {
      return &symbol_table.entries[i];
    }
  }
  return NULL;
}

void set_symbol_type(const char *lexeme, Type type) {
  SymbolEntry *entry = lookup_symbol(lexeme);
  if (entry != NULL) {
    entry->data_type = type;
  }
}

void print_symbol_table() {
  printf("\n=== Tabela de Simbolos ===\n");
  printf("%-20s %-15s %-15s %-10s %-8s %-8s\n", "Lexema", "Tipo Token",
         "Tipo Dado", "Valor", "Linha", "Coluna");
  printf(
      "------------------------------------------------------------------------\n");
  for (int i = 0; i < symbol_table.size; i++) {
    SymbolEntry *entry = &symbol_table.entries[i];
    printf("%-20s ", entry->lexeme);
    switch (entry->token_type) {
    case TOKEN_ID:
      printf("%-15s ", "Identificador");
      break;
    case TOKEN_NUMBER:
      printf("%-15s ", "Numero");
      break;
    default:
      printf("%-15s ", "N/A");
      break;
    }
    
    switch(entry->data_type) {
        case INT: printf("%-15s ", "Int"); break;
        case FLOAT: printf("%-15s ", "Float"); break;
        case CHAR: printf("%-15s ", "Char"); break;
        case UNDEFINED: printf("%-15s ", "Indefinido"); break;
    }

    if (entry->has_value) {
      switch (entry->data_type) {
      case INT:
        printf("%-10d ", entry->value.int_value);
        break;
      case FLOAT:
        printf("%-10.2f ", entry->value.float_value);
        break;
      case CHAR:
        printf("%-10c ", entry->value.char_value);
        break;
      default:
        printf("%-10s ", "N/A");
      }
    } else {
      printf("%-10s ", "N/A");
    }
    printf("%-8d %-8d\n", entry->line, entry->column);
  }
}