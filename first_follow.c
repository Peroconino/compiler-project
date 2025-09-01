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
    if (symbol == ' ') return;
    for (int i = 0; i < *count; i++) {
        if (set[i] == symbol) return;
    }
    set[(*count)++] = symbol;
}

static void compute_first_recursive(char X) {
    NonTerminalSet *nt = find_set(X);
    if (!nt) return;
    if (nt->first_count > 0) return;

    for (int i = 0; i < prod_count; i++) {
        if (productions[i].lhs == X) {
            char *rhs = productions[i].rhs;
            int len = strlen(rhs);

            if (len == 1 && rhs[0] == 'e') {
                add_symbol(nt->first, &nt->first_count, 'e');
            } else {
                for (int j = 0; j < len; j++) {
                    char Y = rhs[j];
                    if (Y >= 'A' && Y <= 'Z') {
                        compute_first_recursive(Y);
                        NonTerminalSet *Yset = find_set(Y);
                        bool has_epsilon = false;
                        for (int k = 0; k < Yset->first_count; k++) {
                            if (Yset->first[k] == 'e') {
                                has_epsilon = true;
                            } else {
                                add_symbol(nt->first, &nt->first_count, Yset->first[k]);
                            }
                        }
                        if (!has_epsilon) break;
                    } else {
                        add_symbol(nt->first, &nt->first_count, Y);
                        break;
                    }
                }
            }
        }
    }
}

static void compute_first() {
    for (int i = 0; i < nonterm_count; i++) {
        compute_first_recursive(nonterminals[i].symbol);
    }
}

static void compute_follow() {
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
                if (!(B >= 'A' && B <= 'Z')) continue;

                NonTerminalSet *Bset = find_set(B);
                NonTerminalSet *Aset = find_set(A);

                if (j + 1 < len) {
                    char beta = rhs[j + 1];
                    if (!(beta >= 'A' && beta <= 'Z')) {
                        int old_count = Bset->follow_count;
                        add_symbol(Bset->follow, &Bset->follow_count, beta);
                        if (old_count != Bset->follow_count) changed = true;
                    } else {
                        NonTerminalSet *Cset = find_set(beta);
                        bool has_epsilon = false;
                        for (int k = 0; k < Cset->first_count; k++) {
                            if (Cset->first[k] == 'e') {
                                has_epsilon = true;
                            } else {
                                int old_count = Bset->follow_count;
                                add_symbol(Bset->follow, &Bset->follow_count, Cset->first[k]);
                                if (old_count != Bset->follow_count) changed = true;
                            }
                        }
                        if (has_epsilon) {
                            for (int k = 0; k < Aset->follow_count; k++) {
                                int old_count = Bset->follow_count;
                                add_symbol(Bset->follow, &Bset->follow_count, Aset->follow[k]);
                                if (old_count != Bset->follow_count) changed = true;
                            }
                        }
                    }
                } else {
                    if (A != B) {
                        for (int k = 0; k < Aset->follow_count; k++) {
                            int old_count = Bset->follow_count;
                            add_symbol(Bset->follow, &Bset->follow_count, Aset->follow[k]);
                            if (old_count != Bset->follow_count) changed = true;
                        }
                    }
                }
            }
        }
    }
}

void compute_first_follow() {
    compute_first();
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
        for (int j = 0; j < nonterminals[i].follow_count; j++) { // Linha corrigida
            printf("%c ", nonterminals[i].follow[j]);
        }
        printf("}\n\n");
    }
}