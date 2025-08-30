#ifndef FIRST_FOLLOW_H
#define FIRST_FOLLOW_H

#define MAX_SYMBOLS 50
#define MAX_PRODUCTIONS 200
#define MAX_RHS 50

typedef struct {
    char lhs;                // não-terminal
    char rhs[MAX_RHS];       // produção (sequência de símbolos)
} Production;

typedef struct {
    char symbol;             // não-terminal
    char first[MAX_SYMBOLS];
    int first_count;
    char follow[MAX_SYMBOLS];
    int follow_count;
} NonTerminalSet;

void add_production(char lhs, const char *rhs);
void compute_first_follow();
void print_first_follow();

#endif
