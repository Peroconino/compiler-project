#include "ast.h"
#include <stdlib.h>
#include <string.h>

ASTNode* create_node(NodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    
    // Inicializa os campos específicos baseado no tipo
    switch(type) {
        case NODE_PROGRAM:
            node->data.program.program_name = NULL;
            node->data.program.block = NULL;
            break;
        case NODE_BLOCK:
            node->data.block.declarations = NULL;
            node->data.block.decl_count = 0;
            node->data.block.commands = NULL;
            node->data.block.cmd_count = 0;
            break;
        case NODE_DECLARATION:
            node->data.declaration.identifiers = NULL;
            node->data.declaration.id_count = 0;
            break;
        case NODE_IF:
            node->data.if_stmt.condition = NULL;
            node->data.if_stmt.then_body = NULL;
            node->data.if_stmt.else_body = NULL;
            break;
        case NODE_ASSIGNMENT:
            node->data.assignment.identifier = NULL;
            node->data.assignment.value = NULL;
            break;
        case NODE_BINARY_EXPR:
            node->data.binary_expr.left = NULL;
            node->data.binary_expr.right = NULL;
            break;
        case NODE_IDENTIFIER:
            node->data.identifier.name = NULL;
            break;
    }
    return node;
}

void free_ast(ASTNode* node) {
    if (!node) return;
    
    switch(node->type) {
        case NODE_PROGRAM:
            free(node->data.program.program_name);
            free_ast(node->data.program.block);
            break;
        case NODE_BLOCK:
            for(int i = 0; i < node->data.block.decl_count; i++) {
                free_ast(node->data.block.declarations[i]);
            }
            free(node->data.block.declarations);
            for(int i = 0; i < node->data.block.cmd_count; i++) {
                free_ast(node->data.block.commands[i]);
            }
            free(node->data.block.commands);
            break;
        case NODE_DECLARATION:
            for(int i = 0; i < node->data.declaration.id_count; i++) {
                free(node->data.declaration.identifiers[i]);
            }
            free(node->data.declaration.identifiers);
            break;
        case NODE_IF:
            free_ast(node->data.if_stmt.condition);
            free_ast(node->data.if_stmt.then_body);
            free_ast(node->data.if_stmt.else_body);
            break;
        case NODE_ASSIGNMENT:
            free(node->data.assignment.identifier);
            free_ast(node->data.assignment.value);
            break;
        case NODE_BINARY_EXPR:
            free_ast(node->data.binary_expr.left);
            free_ast(node->data.binary_expr.right);
            break;
        case NODE_IDENTIFIER:
            free(node->data.identifier.name);
            break;
    }
    free(node);
}

void print_ast(ASTNode* node, int level) {
    if (!node) return;
    
    for(int i = 0; i < level; i++) printf("  ");
    
    switch(node->type) {
        case NODE_PROGRAM:
            printf("Programa: %s\n", node->data.program.program_name);
            print_ast(node->data.program.block, level + 1);
            break;
        case NODE_BLOCK:
            printf("Bloco:\n");
            for(int i = 0; i < node->data.block.decl_count; i++) {
                print_ast(node->data.block.declarations[i], level + 1);
            }
            for(int i = 0; i < node->data.block.cmd_count; i++) {
                print_ast(node->data.block.commands[i], level + 1);
            }
            break;
        // ... adicionar casos para outros tipos de nós
    }
}
