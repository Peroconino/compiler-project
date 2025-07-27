#include "symbol_table.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

SymbolTable symbol_table;

void init_symbol_table() {
    symbol_table.size = 0;
}

int is_symbol_keyword(const char* lexeme) {
    const char* keywords[] = {
        "faca", "enquanto", "main", "inicio", "fim",
        "caso", "entao", "senao", "repita", "ate",
        "int", "float", "char", "if", "else",
        "while", "for", "return"
    };
    int n = sizeof(keywords)/sizeof(keywords[0]);
    for (int i=0;i<n;i++)
        if (strcmp(lexeme, keywords[i]) == 0)
            return 1;
    return 0;
}

void is_keyword(const char* lexeme, SymbolEntry* entry) {
    if (is_symbol_keyword(lexeme)) {
        entry->token_type = TOKEN_KEYWORD;
        if (strcmp(lexeme, "int") == 0) entry->data_type = INT;
        else if (strcmp(lexeme, "float") == 0) entry->data_type = FLOAT;
        else if (strcmp(lexeme, "char") == 0) entry->data_type = CHAR;
    }
}

SymbolEntry* insert_symbol(const char* lexeme, TokenType type, int line, int column) {
    if (symbol_table.size >= MAX_SYMBOLS) {
        printf("Erro: Tabela de símbolos cheia\n");
        return NULL;
    }
    SymbolEntry* entry = &symbol_table.entries[symbol_table.size++];
    strncpy(entry->lexeme, lexeme, MAX_LEXEME_LENGTH - 1);
    entry->lexeme[MAX_LEXEME_LENGTH-1] = '\0';
    entry->token_type = type;
    entry->has_value = false;
    entry->line = line;
    entry->column = column;
    entry->data_type = UNDEFINED;
    is_keyword(lexeme, entry);

    if (type == TOKEN_NUMBER) {
        if (strchr(lexeme, '.')) {
            entry->value.float_value = atof(lexeme);
            entry->data_type = FLOAT;
        } else {
            entry->value.int_value = atoi(lexeme);
            entry->data_type = INT;
        }
        entry->has_value = true;
    }
    return entry;
}

SymbolEntry* lookup_symbol(const char* lexeme) {
    for (int i = 0; i < symbol_table.size; i++) {
        if (strcmp(symbol_table.entries[i].lexeme, lexeme) == 0) {
            return &symbol_table.entries[i];
        }
    }
    return NULL;
}

void set_symbol_type(const char* lexeme, Type type) {
    SymbolEntry* entry = lookup_symbol(lexeme);
    if (entry != NULL) {
        entry->data_type = type;
    }
}

bool set_symbol_value(const char* lexeme, const char* value_str) {
    SymbolEntry* entry = lookup_symbol(lexeme);
    if (entry == NULL) return false;
    switch (entry->data_type) {
        case INT:
            entry->value.int_value = atoi(value_str);
            break;
        case FLOAT:
            entry->value.float_value = atof(value_str);
            break;
        case CHAR:
            entry->value.char_value = value_str[0];
            break;
        default:
            return false;
    }
    entry->has_value = true;
    return true;
}

void print_symbol_table() {
    printf("\n=== Tabela de Simbolos ===\n");
    printf("%-20s %-15s %-15s %-10s %-8s %-8s\n",
           "Lexema", "Tipo Token", "Tipo Dado", "Valor", "Linha", "Coluna");
    printf("------------------------------------------------------------------\n");
    for (int i = 0; i < symbol_table.size; i++) {
        SymbolEntry* entry = &symbol_table.entries[i];
        printf("%-20s ", entry->lexeme);
        switch (entry->token_type) {
            case TOKEN_ID: printf("%-15s ", "Identificador"); break;
            case TOKEN_NUMBER: printf("%-15s ", "Numero"); break;
            case TOKEN_KEYWORD: printf("%-15s ", "Palavra-chave"); break;
            default: printf("%-15s ", "Outro");
        }
        if (entry->has_value) {
            switch (entry->data_type) {
                case INT:
                    printf("%-15s %-10d ", "Int", entry->value.int_value);
                    break;
                case FLOAT:
                    printf("%-15s %-10.2f ", "Float", entry->value.float_value);
                    break;
                case CHAR:
                    printf("%-15s %-10c ", "Char", entry->value.char_value);
                    break;
                default:
                    printf("%-15s %-10s ", "N/A", "N/A");
            }
        } else {
            printf("%-15s %-10s ", "N/A", "N/A");
        }
        printf("%-8d %-8d\n", entry->line, entry->column);
    }
}
