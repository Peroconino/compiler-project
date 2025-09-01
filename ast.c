#include "ast.h"
#include <stdlib.h>
#include <string.h>

ASTNode* create_node(NodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    
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

static const char* get_type_name(Type type) {
    switch(type) {
        case INT: return "int";
        case FLOAT: return "float";
        case CHAR: return "char";
        default: return "undefined";
    }
}

typedef union {
    Operator op;
    Relop relop;
} OperatorUnion;

static const char* get_operator_str(TokenType type, OperatorUnion op) {
    if (type == TOKEN_OPERATOR) {
        switch(op.op) {
            case SUM: return "+";
            case SUB: return "-";
            case MULT: return "*";
            case DIV: return "/";
            case EXP: return "^";
            default: return "?";
        }
    } else if (type == TOKEN_RELOP) {
        switch(op.relop) {
            case GT: return ">";
            case LT: return "<";
            case GE: return ">=";
            case LE: return "<=";
            case EQ: return "==";
            case NE: return "!=";
            default: return "?";
        }
    }
    return "?";
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
            
        case NODE_DECLARATION:
            printf("Declaração: ");
            for(int i = 0; i < node->data.declaration.id_count; i++) {
                printf("%s%s", node->data.declaration.identifiers[i],
                       i < node->data.declaration.id_count - 1 ? ", " : "");
            }
            printf(" -> %s\n", get_type_name(node->data.declaration.var_type));
            break;
            
        case NODE_IF:
            printf("Se:\n");
            for(int i = 0; i < level + 1; i++) printf("  ");
            printf("Condição:\n");
            print_ast(node->data.if_stmt.condition, level + 2);
            for(int i = 0; i < level + 1; i++) printf("  ");
            printf("Então:\n");
            print_ast(node->data.if_stmt.then_body, level + 2);
            if (node->data.if_stmt.else_body) {
                for(int i = 0; i < level + 1; i++) printf("  ");
                printf("Senão:\n");
                print_ast(node->data.if_stmt.else_body, level + 2);
            }
            break;
            
        case NODE_ASSIGNMENT:
            printf("Atribuição: %s = \n", node->data.assignment.identifier);
            print_ast(node->data.assignment.value, level + 1);
            break;
            
        case NODE_BINARY_EXPR:
            printf("Expressão: ");
            print_ast(node->data.binary_expr.left, 0);
            {
                OperatorUnion op;
                if (node->data.binary_expr.operator_type == TOKEN_OPERATOR)
                    op.op = node->data.binary_expr.op.op;
                else
                    op.relop = node->data.binary_expr.op.relop;
                    
                printf(" %s ", get_operator_str(node->data.binary_expr.operator_type, op));
            }
            print_ast(node->data.binary_expr.right, 0);
            printf("\n");
            break;
            
        case NODE_IDENTIFIER:
            printf("%s", node->data.identifier.name);
            break;
            
        case NODE_NUMBER:
            switch(node->data.number.number_type) {
                case INT:
                    printf("%d", node->data.number.value.int_value);
                    break;
                case FLOAT:
                    printf("%f", node->data.number.value.float_value);
                    break;
                case CHAR:
                    printf("'%c'", node->data.number.value.char_value);
                    break;
                default:
                    printf("???");
            }
            break;
    }
}
