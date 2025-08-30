#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ASTNode* create_node(NodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Erro de alocação de memória para o nó da AST\n");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    node->next = NULL;
    return node;
}

ASTNode* create_program_node(char* name, ASTNode* block) {
    ASTNode* node = create_node(NODE_PROGRAM);
    node->data.program.name = strdup(name);
    node->data.program.block = block;
    return node;
}

ASTNode* create_block_node(ASTNode* declarations, ASTNode* commands) {
    ASTNode* node = create_node(NODE_BLOCK);
    node->data.block.declarations = declarations;
    node->data.block.commands = commands;
    return node;
}

ASTNode* create_var_decl_node(ASTNode* id_list, ASTNode* type) {
    ASTNode* node = create_node(NODE_VAR_DECL);
    node->data.var_decl.id_list = id_list;
    node->data.var_decl.type = type;
    return node;
}

ASTNode* create_type_node(Type type) {
    ASTNode* node = create_node(NODE_TYPE);
    node->data.type_spec.type = type;
    return node;
}

ASTNode* create_if_node(ASTNode* condition, ASTNode* then_branch, ASTNode* else_branch) {
    ASTNode* node = create_node(NODE_IF);
    node->data.if_stmt.condition = condition;
    node->data.if_stmt.then_branch = then_branch;
    node->data.if_stmt.else_branch = else_branch;
    return node;
}

ASTNode* create_assign_node(ASTNode* variable, ASTNode* expression) {
    ASTNode* node = create_node(NODE_ASSIGN);
    node->data.assign.variable = variable;
    node->data.assign.expression = expression;
    return node;
}

ASTNode* create_binary_op_node(Token* op, ASTNode* left, ASTNode* right) {
    ASTNode* node = create_node(NODE_BINARY_OP);
    node->data.binary_op.op = op;
    node->data.binary_op.left = left;
    node->data.binary_op.right = right;
    return node;
}

ASTNode* create_identifier_node(char* value) {
    ASTNode* node = create_node(NODE_IDENTIFIER);
    node->data.identifier.value = strdup(value);
    return node;
}

ASTNode* create_number_node(char* value, Type numType) {
    ASTNode* node = create_node(NODE_NUMBER);
    node->data.number.value = strdup(value);
    node->data.number.numberType = numType;
    return node;
}

ASTNode* create_char_literal_node(char* value) {
    ASTNode* node = create_node(NODE_CHAR_LITERAL);
    node->data.char_literal.value = strdup(value);
    return node;
}

void print_ast(ASTNode* node, int level) {
    if (!node) return;

    for (int i = 0; i < level; i++) printf("  ");

    switch (node->type) {
        case NODE_PROGRAM:
            printf("Programa: %s\n", node->data.program.name);
            print_ast(node->data.program.block, level + 1);
            break;
        case NODE_BLOCK:
            printf("Bloco\n");
            print_ast(node->data.block.declarations, level + 1);
            print_ast(node->data.block.commands, level + 1);
            break;
        case NODE_VAR_DECL:
            printf("Declaracao Variavel\n");
            print_ast(node->data.var_decl.id_list, level + 1);
            print_ast(node->data.var_decl.type, level + 1);
            break;
        case NODE_TYPE:
            printf("Tipo: %s\n", node->data.type_spec.type == INT ? "int" : (node->data.type_spec.type == FLOAT ? "float" : "char"));
            break;
        case NODE_IDENTIFIER:
            printf("ID: %s\n", node->data.identifier.value);
            break;
        default:
            printf("Nó não reconhecido: %d\n", node->type);
    }
    if (node->next) {
        print_ast(node->next, level);
    }
}

void free_ast(ASTNode* node) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_PROGRAM:
            free(node->data.program.name);
            free_ast(node->data.program.block);
            break;
        case NODE_BLOCK:
            free_ast(node->data.block.declarations);
            free_ast(node->data.block.commands);
            break;
        case NODE_VAR_DECL:
            free_ast(node->data.var_decl.id_list);
            free_ast(node->data.var_decl.type);
            break;
        case NODE_IDENTIFIER:
            free(node->data.identifier.value);
            break;
        case NODE_NUMBER:
        case NODE_CHAR_LITERAL:
             free(node->data.number.value);
             break;
    }

    free_ast(node->next);
    free(node);
}