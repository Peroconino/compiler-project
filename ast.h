#ifndef AST_H
#define AST_H

#include "lexer.h"

typedef enum {
    NODE_PROGRAM,
    NODE_BLOCK,
    NODE_VAR_DECL,
    NODE_TYPE,
    NODE_COMMAND_SEQUENCE,
    NODE_IF,
    NODE_ASSIGN,
    NODE_BINARY_OP,
    NODE_IDENTIFIER,
    NODE_NUMBER,
    NODE_CHAR_LITERAL
} NodeType;

typedef struct ASTNode {
    NodeType type;
    struct ASTNode *next; 
    union {
        struct {
            char* name;
            struct ASTNode* block;
        } program;

        struct {
            struct ASTNode* declarations;
            struct ASTNode* commands;
        } block;

        struct {
            struct ASTNode* id_list;
            struct ASTNode* type;
        } var_decl;
        
        struct {
            Type type;
        } type_spec;

        struct {
            struct ASTNode* condition;
            struct ASTNode* then_branch;
            struct ASTNode* else_branch; 
        } if_stmt;

        struct {
            struct ASTNode* variable;
            struct ASTNode* expression;
        } assign;

        struct {
            Token* op;
            struct ASTNode* left;
            struct ASTNode* right;
        } binary_op;

        struct {
            char* value;
        } identifier;

        struct {
            char* value;
            Type numberType;
        } number;

        struct {
            char* value;
        } char_literal;

    } data;
} ASTNode;


ASTNode* create_program_node(char* name, ASTNode* block);
ASTNode* create_block_node(ASTNode* declarations, ASTNode* commands);
ASTNode* create_var_decl_node(ASTNode* id_list, ASTNode* type);
ASTNode* create_type_node(Type type);
ASTNode* create_if_node(ASTNode* condition, ASTNode* then_branch, ASTNode* else_branch);
ASTNode* create_assign_node(ASTNode* variable, ASTNode* expression);
ASTNode* create_binary_op_node(Token* op, ASTNode* left, ASTNode* right);
ASTNode* create_identifier_node(char* value);
ASTNode* create_number_node(char* value, Type numType);
ASTNode* create_char_literal_node(char* value);

void free_ast(ASTNode* node);

void print_ast(ASTNode* node, int level);

#endif