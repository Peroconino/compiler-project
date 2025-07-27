#ifndef SYMBOL_TABLE_H_INCLUDED
#define SYMBOL_TABLE_H_INCLUDED

#include "lexer.h"
#include <stdbool.h>

#define MAX_SYMBOLS 1000
#define MAX_LEXEME_LENGTH 50

typedef struct {
    char lexeme[MAX_LEXEME_LENGTH];
    TokenType token_type;
    union {
        int int_value;
        float float_value;
        char char_value;
    } value;
    Type data_type;
    bool has_value;
    int line;
    int column;
} SymbolEntry;

typedef struct {
    SymbolEntry entries[MAX_SYMBOLS];
    int size;
} SymbolTable;

void init_symbol_table();
SymbolEntry* insert_symbol(const char* lexeme, TokenType type, int line, int column);
SymbolEntry* lookup_symbol(const char* lexeme);
void print_symbol_table();
void set_symbol_type(const char* lexeme, Type type);
bool set_symbol_value(const char* lexeme, const char* value_str);

#endif // SYMBOL_TABLE_H_INCLUDED
