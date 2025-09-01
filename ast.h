#ifndef AST_H
#define AST_H

#include "lexer.h"
#include <stdlib.h>

typedef enum {
    NODE_PROGRAM,
    NODE_BLOCK,
    NODE_DECLARATION,
    NODE_IF,
    NODE_ASSIGNMENT,
    NODE_BINARY_EXPR,
    NODE_IDENTIFIER,
    NODE_NUMBER,
} NodeType;

typedef struct ASTNode {
    NodeType type;
    union {
        // Para NODE_PROGRAM
        struct {
            char *program_name;
            struct ASTNode *block;
        } program;
        
        // Para NODE_BLOCK
        struct {
            struct ASTNode **declarations; // Array de declarações
            int decl_count;
            struct ASTNode **commands;     // Array de comandos
            int cmd_count;
        } block;
        
        // Para NODE_DECLARATION
        struct {
            char **identifiers;  // Array de identificadores
            int id_count;
            Type var_type;
        } declaration;
        
        // Para NODE_IF
        struct {
            struct ASTNode *condition;
            struct ASTNode *then_body;
            struct ASTNode *else_body;
        } if_stmt;
        
        // Para NODE_ASSIGNMENT
        struct {
            char *identifier;
            struct ASTNode *value;
        } assignment;
        
        // Para NODE_BINARY_EXPR
        struct {
            struct ASTNode *left;
            struct ASTNode *right;
            TokenType operator_type;
            union {
                Operator op;
                Relop relop;
            } op;
        } binary_expr;
        
        // Para NODE_IDENTIFIER
        struct {
            char *name;
        } identifier;
        
        // Para NODE_NUMBER
        struct {
            union {
                int int_value;
                float float_value;
                char char_value;
            } value;
            Type number_type;
        } number;
    } data;
    
    int line;
    int column;
} ASTNode;

ASTNode* create_node(NodeType type);
void free_ast(ASTNode* node);
void print_ast(ASTNode* node, int level);  // Adicionar este protótipo

#endif
