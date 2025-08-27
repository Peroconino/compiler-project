#include "lexer.h"
#include "symbol_table.h"
#include "ast.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// --- Estrutura do Parser com Lookahead ---
static Token *currentToken;
static Token *lookaheadToken = NULL; // Buffer para um token de lookahead

// Definição da variável global de erro
bool hasError = false;

static ASTNode* current_ast = NULL;

// --- Protótipos das Funções ---
ASTNode* procedimento_programa();
ASTNode* procedimento_bloco();
ASTNode* procedimento_decl_vars_linha();
ASTNode* procedimento_seq_comandos();
ASTNode* procedimento_comando();
ASTNode* procedimento_atribuicao();
ASTNode* procedimento_selecao();
ASTNode* procedimento_corpo();
ASTNode* procedimento_expr();

// --- Funções de Controle de Token ---

// Avança para o próximo token, usando o buffer de lookahead se não estiver vazio
void consume_token() {
  if (hasError) return;
  
  if (lookaheadToken != NULL) {
    if(currentToken) free(currentToken);
    currentToken = lookaheadToken;
    lookaheadToken = NULL;
  } else {
    if(currentToken) free(currentToken);
    currentToken = getNextToken();
  }
}

// Espia o próximo token sem consumi-lo
Token* peek_ahead() {
    if (lookaheadToken == NULL) {
        lookaheadToken = getNextToken();
    }
    return lookaheadToken;
}


// --- Funções de Controle e Erro ---

// Modifica a função de erro para incluir mais contexto
void syntax_error(const char *expected) {
    if (!hasError) {
        printf("Erro Sintatico (Linha %d, Coluna %d):\n", currentToken->line, currentToken->column);
        printf("  Esperado: %s\n", expected);
        printf("  Encontrado: '%s'\n", currentToken->value);
        printf("  Contexto: ");
        
        // Imprime o contexto da linha onde ocorreu o erro
        int pos = state.token_start_pos;
        while(pos > 0 && state.source_buffer[pos-1] != '\n') pos--;
        while(state.source_buffer[pos] != '\n' && state.source_buffer[pos] != '\0') {
            putchar(state.source_buffer[pos++]);
        }
        printf("\n");
        
        hasError = true;
    }
}

void match(TokenType type, int sub_type) {
    if (hasError) return;

    bool types_match = (currentToken->type == type);
    bool subtypes_match = false;
    
    if (types_match) {
        switch(type) {
            case TOKEN_KEYWORD:
                subtypes_match = (currentToken->keyword == sub_type);
                break;
            case TOKEN_PUNCTUATION:
                subtypes_match = (currentToken->pontuacao == sub_type);
                break;
            case TOKEN_OPERATOR:
                subtypes_match = (currentToken->operador == sub_type);
                break;
            case TOKEN_RELOP:
            case TOKEN_ID:
            case TOKEN_NUMBER:
            case TOKEN_EOF:
                subtypes_match = true;
                break;
            default:
                subtypes_match = false;
        }
    }

    if (types_match && subtypes_match) {
        consume_token();
    } else {
        char expected_str[100] = "";
        
        switch(type) {
            case TOKEN_KEYWORD:
                switch(sub_type) {
                    case IF: strcpy(expected_str, "'caso'"); break;
                    case THEN: strcpy(expected_str, "'entao'"); break;
                    case ELSE: strcpy(expected_str, "'senao'"); break;
                    case MAIN: strcpy(expected_str, "'main'"); break;
                    case BEGIN: strcpy(expected_str, "'inicio'"); break;
                    case END: strcpy(expected_str, "'fim'"); break;
                    case TYPE_INT: strcpy(expected_str, "'int'"); break;
                    case TYPE_CHAR: strcpy(expected_str, "'char'"); break;
                    case TYPE_FLOAT: strcpy(expected_str, "'float'"); break;
                    default: strcpy(expected_str, "palavra-chave");
                }
                break;
            case TOKEN_PUNCTUATION:
                switch(sub_type) {
                    case END_EXP: strcpy(expected_str, "';'"); break;
                    case MUL_VARS: strcpy(expected_str, "','"); break;
                    case DECLARATION: strcpy(expected_str, "'->'"); break;
                    default: strcpy(expected_str, "pontuação");
                }
                break;
            case TOKEN_OPERATOR:
                switch(sub_type) {
                    case ASSIGN: strcpy(expected_str, "'='"); break;
                    case PARESQ: strcpy(expected_str, "'('"); break;
                    case PARDIR: strcpy(expected_str, "')'"); break;
                    default: strcpy(expected_str, "operador");
                }
                break;
            case TOKEN_ID:
                strcpy(expected_str, "identificador");
                break;
            case TOKEN_NUMBER:
                strcpy(expected_str, "número");
                break;
            default:
                strcpy(expected_str, "token desconhecido");
        }
        
        syntax_error(expected_str);
        exit(1);
    }
}

// --- Implementação dos Procedimentos de Análise ---

// expr -> ID | NUMERO | (expr) | expr op expr
ASTNode* procedimento_expr() {
    if (hasError) return NULL;
    
    ASTNode* left = NULL;
    
    // Trata (expr)
    if(currentToken->type == TOKEN_OPERATOR && currentToken->operador == PARESQ) {
        match(TOKEN_OPERATOR, PARESQ);
        left = procedimento_expr();
        match(TOKEN_OPERATOR, PARDIR);
    } 
    // Trata operandos iniciais
    else if(currentToken->type == TOKEN_ID) {
        left = create_node(NODE_IDENTIFIER);
        left->data.identifier.name = strdup(currentToken->value);
        match(TOKEN_ID, 0);
    }
    else if(currentToken->type == TOKEN_NUMBER) {
        left = create_node(NODE_NUMBER);
        left->data.number.number_type = currentToken->numberType;
        // ... converter e armazenar o valor
        match(TOKEN_NUMBER, 0);
    } else {
        syntax_error("um identificador, numero, char ou '('");
        return NULL;
    }
    
    // Trata operadores binários
    if(currentToken->type == TOKEN_RELOP || 
       (currentToken->type == TOKEN_OPERATOR && (
            currentToken->operador == SUM || currentToken->operador == SUB ||
            currentToken->operador == MULT || currentToken->operador == DIV ||
            currentToken->operador == EXP
       ))) {
        ASTNode* binOp = create_node(NODE_BINARY_EXPR);
        binOp->data.binary_expr.left = left;
        binOp->data.binary_expr.operator_type = currentToken->type;
        if(currentToken->type == TOKEN_RELOP)
            binOp->data.binary_expr.op.relop = currentToken->relop;
        else
            binOp->data.binary_expr.op.op = currentToken->operador;
        
        consume_token();
        binOp->data.binary_expr.right = procedimento_expr();
        return binOp;
    }
    
    return left;
}

// atribuicao -> ID '=' expr ';'
ASTNode* procedimento_atribuicao() {
    if (hasError) return NULL;
    
    ASTNode* node = create_node(NODE_ASSIGNMENT);
    node->data.assignment.identifier = strdup(currentToken->value);
    
    match(TOKEN_ID, 0);
    match(TOKEN_OPERATOR, ASSIGN);
    
    node->data.assignment.value = procedimento_expr();
    match(TOKEN_PUNCTUATION, END_EXP);
    
    return node;
}

// corpo -> comando | bloco
ASTNode* procedimento_corpo() {
    if (hasError) return NULL;
    
    if (currentToken->type == TOKEN_KEYWORD && currentToken->keyword == BEGIN) {
        return procedimento_bloco();
    } else {
        return procedimento_comando();
    }
}

// selecao -> 'caso' '(' expr ')' 'entao' corpo ('senao' corpo)?
ASTNode* procedimento_selecao() {
    if (hasError) return NULL;
    
    ASTNode* node = create_node(NODE_IF);
    
    match(TOKEN_KEYWORD, IF);
    match(TOKEN_OPERATOR, PARESQ);
    node->data.if_stmt.condition = procedimento_expr();
    match(TOKEN_OPERATOR, PARDIR);
    match(TOKEN_KEYWORD, THEN);
    node->data.if_stmt.then_body = procedimento_corpo();

    if (currentToken->type == TOKEN_KEYWORD && currentToken->keyword == ELSE) {
        match(TOKEN_KEYWORD, ELSE);
        node->data.if_stmt.else_body = procedimento_corpo();
    } else {
        node->data.if_stmt.else_body = NULL;
    }
    
    return node;
}

// comando -> atribuicao | selecao
ASTNode* procedimento_comando() {
    if (hasError) return NULL;
    
    if (currentToken->type == TOKEN_ID) {
        return procedimento_atribuicao();
    } else if (currentToken->type == TOKEN_KEYWORD && currentToken->keyword == IF) {
        return procedimento_selecao();
    }
    
    syntax_error("o inicio de um comando valido (ID, 'caso', etc.)");
    return NULL;
}

// seq_comandos -> (comando)*
ASTNode* procedimento_seq_comandos() {
    if (hasError) return NULL;
    
    ASTNode* block = create_node(NODE_BLOCK);
    block->data.block.commands = malloc(sizeof(ASTNode*) * 100); // Tamanho arbitrário
    block->data.block.cmd_count = 0;
    
    while (currentToken->type != TOKEN_KEYWORD || currentToken->keyword != END) {
        if(currentToken->type == TOKEN_EOF) {
            syntax_error("'fim'");
            return block;
        }
        if (currentToken->type == TOKEN_KEYWORD && currentToken->keyword == ELSE) {
            break;
        }
        ASTNode* cmd = procedimento_comando();
        if(cmd) {
            block->data.block.commands[block->data.block.cmd_count++] = cmd;
        }
        if(hasError) break;
    }
    
    return block;
}

// Analisa uma única linha de declaração de variáveis
ASTNode* procedimento_decl_vars_linha() {
    if (hasError) return NULL;

    ASTNode* decl_node = create_node(NODE_DECLARATION);
    decl_node->data.declaration.identifiers = malloc(sizeof(char*) * 10);
    decl_node->data.declaration.id_count = 0;

    // Verifica se começa com um identificador
    if (currentToken->type != TOKEN_ID) {
        syntax_error("um identificador");
        free_ast(decl_node);
        return NULL;
    }

    // Guarda o primeiro identificador
    decl_node->data.declaration.identifiers[decl_node->data.declaration.id_count++] = 
        strdup(currentToken->value);
    match(TOKEN_ID, 0);

    // Coleta outros identificadores se houver
    while(currentToken->type == TOKEN_PUNCTUATION && currentToken->pontuacao == MUL_VARS) {
        match(TOKEN_PUNCTUATION, MUL_VARS);
        if (currentToken->type != TOKEN_ID) {
            syntax_error("um identificador após ','");
            free_ast(decl_node);
            return NULL;
        }
        decl_node->data.declaration.identifiers[decl_node->data.declaration.id_count++] = 
            strdup(currentToken->value);
        match(TOKEN_ID, 0);
    }
    
    match(TOKEN_PUNCTUATION, DECLARATION);

    // Define o tipo das variáveis
    if (currentToken->type == TOKEN_KEYWORD) {
        switch(currentToken->keyword) {
            case TYPE_INT: decl_node->data.declaration.var_type = INT; break;
            case TYPE_CHAR: decl_node->data.declaration.var_type = CHAR; break;
            case TYPE_FLOAT: decl_node->data.declaration.var_type = FLOAT; break;
            default:
                syntax_error("'int', 'char' ou 'float'");
                free_ast(decl_node);
                return NULL;
        }
        match(TOKEN_KEYWORD, currentToken->keyword);

        // Atualiza a tabela de símbolos
        for(int i = 0; i < decl_node->data.declaration.id_count; i++) {
            set_symbol_type(decl_node->data.declaration.identifiers[i], 
                          decl_node->data.declaration.var_type);
        }
    }

    match(TOKEN_PUNCTUATION, END_EXP);
    return decl_node;
}

// bloco -> 'inicio' decl_vars seq_comandos 'fim'
ASTNode* procedimento_bloco() {
    if (hasError) return NULL;
    
    match(TOKEN_KEYWORD, BEGIN);

    ASTNode* block_node = create_node(NODE_BLOCK);
    block_node->data.block.declarations = malloc(sizeof(ASTNode*) * 100);
    block_node->data.block.decl_count = 0;
    block_node->data.block.commands = malloc(sizeof(ASTNode*) * 100);
    block_node->data.block.cmd_count = 0;

    // Seção de declarações (opcional)
    while (currentToken->type == TOKEN_ID) {
        Token *next = peek_ahead();
        if (next->type == TOKEN_PUNCTUATION && 
            (next->pontuacao == MUL_VARS || next->pontuacao == DECLARATION)) {
            ASTNode* decl = procedimento_decl_vars_linha();
            if (decl) {
                block_node->data.block.declarations[block_node->data.block.decl_count++] = decl;
            }
        } else {
            break;
        }
    }

    // Seção de comandos
    ASTNode* commands = procedimento_seq_comandos();
    if (commands) {
        // Transfere os comandos do nó temporário para o bloco
        for(int i = 0; i < commands->data.block.cmd_count; i++) {
            block_node->data.block.commands[block_node->data.block.cmd_count++] = 
                commands->data.block.commands[i];
        }
        free(commands->data.block.commands);
        free(commands);
    }

    match(TOKEN_KEYWORD, END);
    return block_node;
}

// programa -> 'main' ID '()' bloco
ASTNode* procedimento_programa() {
    consume_token();
    
    ASTNode* program_node = create_node(NODE_PROGRAM);
    current_ast = program_node;
    
    match(TOKEN_KEYWORD, MAIN);
    program_node->data.program.program_name = strdup(currentToken->value);
    match(TOKEN_ID, 0);
    match(TOKEN_OPERATOR, PARESQ);
    match(TOKEN_OPERATOR, PARDIR);
    
    program_node->data.program.block = procedimento_bloco();
    
    if (!hasError) {
        printf("\nAnalise Sintatica concluida com sucesso!\n");
        // Aqui você pode adicionar funções para imprimir ou processar a árvore
    }
    
    if(currentToken) free(currentToken);
    if(lookaheadToken) free(lookaheadToken);
    return program_node; // Retorna a AST gerada
}
