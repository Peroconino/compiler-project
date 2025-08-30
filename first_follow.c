#include "first_follow.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static Production productions[MAX_PRODUCTIONS];
static int prod_count = 0;
static NonTerminalSet nonterminals[MAX_SYMBOLS];
static int nonterm_count = 0;

void add_production(char lhs, const char *rhs) {
    productions[prod_count].lhs = lhs;
    strcpy(productions[prod_count].rhs, rhs);
    prod_count++;

    // Se não-terminal ainda não existe na lista
    bool exists = false;
    for (int i = 0; i < nonterm_count; i++) {
        if (nonterminals[i].symbol == lhs) {
            exists = true;
            break;
        }
    }
    if (!exists) {
        nonterminals[nonterm_count].symbol = lhs;
        nonterminals[nonterm_count].first_count = 0;
        nonterminals[nonterm_count].follow_count = 0;
        nonterm_count++;
    }
}

static NonTerminalSet* find_set(char symbol) {
    for (int i = 0; i < nonterm_count; i++) {
        if (nonterminals[i].symbol == symbol) return &nonterminals[i];
    }
    return NULL;
}

static void add_symbol(char *set, int *count, char symbol) {
    if (symbol == ' ') return; // ignora espaços
    for (int i = 0; i < *count; i++) {
        if (set[i] == symbol) return;
    }
    set[(*count)++] = symbol;
}

static void compute_first(char X);

static void compute_first(char X) {
    NonTerminalSet *nt = find_set(X);
    if (!nt) return;

    for (int i = 0; i < prod_count; i++) {
        if (productions[i].lhs == X) {
            char Y = productions[i].rhs[0];

            if (Y == 'e') { // epsilon
                add_symbol(nt->first, &nt->first_count, 'e');
            }
            else if (!(Y >= 'A' && Y <= 'Z')) { // terminal
                add_symbol(nt->first, &nt->first_count, Y);
            } else { // não-terminal
                compute_first(Y);
                NonTerminalSet *Yset = find_set(Y);
                for (int j = 0; j < Yset->first_count; j++) {
                    add_symbol(nt->first, &nt->first_count, Yset->first[j]);
                }
            }
        }
    }
}

static void compute_follow() {
    // símbolo inicial recebe $
    add_symbol(find_set(productions[0].lhs)->follow,
               &find_set(productions[0].lhs)->follow_count, '$');

    bool changed = true;
    while (changed) {
        changed = false;
        for (int i = 0; i < prod_count; i++) {
            char A = productions[i].lhs;
            char *rhs = productions[i].rhs;
            int len = strlen(rhs);

            for (int j = 0; j < len; j++) {
                char B = rhs[j];
                if (!(B >= 'A' && B <= 'Z')) continue; // só para não-terminais

                NonTerminalSet *Bset = find_set(B);

                if (j + 1 < len) {
                    char beta = rhs[j + 1];
                    if (!(beta >= 'A' && beta <= 'Z')) { // terminal
                        add_symbol(Bset->follow, &Bset->follow_count, beta);
                    } else { // não-terminal
                        NonTerminalSet *Cset = find_set(beta);
                        for (int k = 0; k < Cset->first_count; k++) {
                            if (Cset->first[k] != 'e') {
                                add_symbol(Bset->follow, &Bset->follow_count, Cset->first[k]);
                            }
                        }
                        if (Cset->first_count > 0 && Cset->first[Cset->first_count-1] == 'e') {
                            NonTerminalSet *Aset = find_set(A);
                            for (int k = 0; k < Aset->follow_count; k++) {
                                add_symbol(Bset->follow, &Bset->follow_count, Aset->follow[k]);
                            }
                        }
                    }
                } else { // último símbolo
                    NonTerminalSet *Aset = find_set(A);
                    for (int k = 0; k < Aset->follow_count; k++) {
                        add_symbol(Bset->follow, &Bset->follow_count, Aset->follow[k]);
                    }
                }
            }
        }
    }
}

void compute_first_follow() {
    for (int i = 0; i < nonterm_count; i++) {
        compute_first(nonterminals[i].symbol);
    }
    compute_follow();
}

void print_first_follow() {
    for (int i = 0; i < nonterm_count; i++) {
        printf("FIRST(%c) = { ", nonterminals[i].symbol);
        for (int j = 0; j < nonterminals[i].first_count; j++) {
            printf("%c ", nonterminals[i].first[j]);
        }
        printf("}\n");

        printf("FOLLOW(%c) = { ", nonterminals[i].symbol);
        for (int j = 0; j < nonterminals[i].follow_count; j++) {
            printf("%c ", nonterminals[i].follow[j]);
        }
        printf("}\n\n");
    }
}
