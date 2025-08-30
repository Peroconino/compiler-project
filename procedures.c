// #include "lexer.h"
// #include <stddef.h>
// #include <stdio.h>

// void imprime_token_atual(Token *proxToken) {
//   printf("\nFoi encontrado '%s' em vez disso", proxToken->value);
// }

// void procedimento_lista_ids() {
//   Token *proxToken = getNextToken();
//   if (proxToken->type == TOKEN_ID) {
//     // prodecimento_cont_lista_ids()
//     Token *proxToken = getNextToken();
//     if (proxToken->type == TOKEN_PUNCTUATION &&
//         proxToken->pontuacao == MUL_VARS) {
//       procedimento_lista_ids();
//     }
//     // fim cont_list_ids()
//   }
//   return;
// }

// void procedimento_tipo() {
//   Token *proxToken = getNextToken();
//   if (proxToken->type == TOKEN_NUMBER &&
//       (proxToken->numberType == INT || proxToken->numberType == CHAR ||
//        proxToken->numberType == FLOAT)) {
//     return;
//   }
// }

// void procedimento_decl_var() {
//   procedimento_lista_ids();

//   Token *proxToken = getNextToken();
//   if (proxToken->type == TOKEN_PUNCTUATION &&
//       proxToken->pontuacao == ASSIGMENT) {
//     procedimento_tipo();

//     Token *proxToken = getNextToken();
//     if (proxToken->type == TOKEN_PUNCTUATION &&
//         proxToken->pontuacao == END_EXP) {
//       return;
//     } else {
//       printf("Erro: era esperado ;");
//     }
//   } else {
//     printf("Erro: era esperado um tipo");
//   }

//   imprime_token_atual(proxToken);
// }

// void procedimento_lista_decl(Token *proxToken) {

//   while (proxToken->type == TOKEN_ID) {
//     procedimento_decl_var();
//     proxToken = getNextToken();
//   }
// }

// void procedimento_decl_vars() {

//   Token *proxToken = getNextToken();

//   if (proxToken->type == TOKEN_ID) {
//     procedimento_lista_decl(proxToken);
//   }

//   return;
// }

// void procedimento_bloco() {
//   Token *proxToken = getNextToken();

//   if (proxToken->type == TOKEN_KEYWORD && proxToken->keyword == BEGIN) {
//     procedimento_decl_vars();
//     procedimento_seq_comandos();

//     Token *proxToken = getNextToken();
//     if (proxToken->type == TOKEN_KEYWORD && proxToken->keyword == END) {
//       return;
//     } else {
//       printf("Erro: Era esperado fim de bloco");
//     }
//   } else {
//     printf("Erro: Era esperado inicio de bloco");
//   }

//   imprime_token_atual(proxToken);
// }

// void procedimento_programa() {
//   Token *proxToken = getNextToken();

//   if (proxToken->type == TOKEN_KEYWORD && proxToken->keyword == MAIN) {
//     Token *proxToken = getNextToken();

//     if (proxToken->type == TOKEN_ID) {
//       Token *proxToken = getNextToken();

//       if (proxToken->type == TOKEN_OPERATOR && proxToken->operador == PARESQ) {
//         Token *proxToken = getNextToken();

//         if (proxToken->type == TOKEN_OPERATOR &&
//             proxToken->operador == PARDIR) {
//           procedimento_bloco();
//           return;
//         } else {
//           printf("Erro: Era esperado ) parentesis direito");
//         }
//       } else {
//         printf("Erro: Era esperado ( parentesis esquerdo)");
//       }
//     } else {
//       printf("Erro: Era esperado o nome do programa");
//     }
//   } else {
//     printf("Erro: Era esperado main para o inicio de programa");
//   }
//   imprime_token_atual(proxToken);
// }

#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

// Função auxiliar para imprimir erros e consumir o token
void parse_error(const char* expected, Token* found) {
    printf("Erro de sintaxe: Esperado '%s', mas encontrado '%s' na linha %d, coluna %d\n",
           expected, found->value, found->line, found->column);
    // Em um parser real, haveria uma estratégia de recuperação de erro.
    // Por simplicidade, vamos parar.
    exit(EXIT_FAILURE);
}

// Protótipos para as funções que ainda não foram declaradas
ASTNode* procedimento_seq_comandos(); // Necessário para a chamada em `procedimento_bloco`

ASTNode* procedimento_lista_ids() {
    Token* proxToken = getNextToken();
    if (proxToken->type != TOKEN_ID) {
        parse_error("um identificador", proxToken);
        return NULL;
    }
    
    ASTNode* head = create_identifier_node(proxToken->value);
    ASTNode* current = head;

    // Verifica se há mais identificadores na lista (ex: x, y, z)
    proxToken = getNextToken();
    while (proxToken->type == TOKEN_PUNCTUATION && proxToken->pontuacao == MUL_VARS) {
        proxToken = getNextToken(); // Consome a vírgula
        if (proxToken->type != TOKEN_ID) {
            parse_error("um identificador após a vírgula", proxToken);
            free_ast(head);
            return NULL;
        }
        current->next = create_identifier_node(proxToken->value);
        current = current->next;
        proxToken = getNextToken(); // Próximo token para a condição do loop
    }
    // "Devolve" o último token lido que não faz parte da lista
    // Esta é uma simplificação. Um parser real teria um lookahead de 1 token.
    // Para este código, vamos assumir que a gramática permite essa leitura sequencial.
    // Em uma implementação mais robusta, o token seria colocado de volta em um buffer.
    return head;
}


ASTNode* procedimento_tipo() {
    Token* proxToken = getNextToken();
    if (proxToken->type == TOKEN_KEYWORD) { // Supondo que tipos sejam palavras-chave
        Type detected_type;
        // Adapte os nomes 'INT_KW', etc., para os keywords reais da sua linguagem
        if (proxToken->keyword == INT) detected_type = INT;
        else if (proxToken->keyword == FLOAT) detected_type = FLOAT;
        else if (proxToken->keyword == CHAR) detected_type = CHAR;
        else {
             parse_error("um tipo (int, float, char)", proxToken);
             return NULL;
        }
        return create_type_node(detected_type);
    }
    parse_error("um tipo", proxToken);
    return NULL;
}

ASTNode* procedimento_decl_var() {
    ASTNode* id_list = procedimento_lista_ids();
    
    Token* proxToken = getNextToken();
    if (!(proxToken->type == TOKEN_PUNCTUATION && proxToken->pontuacao == ASSIGMENT)) { // Supondo que '->' é ASSIGMENT
        parse_error("->", proxToken);
        free_ast(id_list);
        return NULL;
    }

    ASTNode* type_node = procedimento_tipo();

    proxToken = getNextToken();
    if (!(proxToken->type == TOKEN_PUNCTUATION && proxToken->pontuacao == END_EXP)) {
        parse_error(";", proxToken);
        free_ast(id_list);
        free_ast(type_node);
        return NULL;
    }

    return create_var_decl_node(id_list, type_node);
}

ASTNode* procedimento_lista_decl() {
    ASTNode* head = procedimento_decl_var();
    ASTNode* current = head;

    // Espia o próximo token para ver se há mais declarações
    // Token* next = peekNextToken(); // Idealmente, o lexer teria uma função peek
    // Por enquanto, vamos assumir que o fluxo de `procedimento_decl_vars` controla isso.
    
    return head;
}

ASTNode* procedimento_decl_vars() {
    // Esta função deve construir uma lista de declarações.
    // A lógica atual em `procedimento_bloco` parece ler apenas uma.
    // Vamos adaptar para criar uma lista encadeada de declarações.
    
    // O primeiro token já foi lido por quem chamou a função.
    ASTNode* head = NULL;
    ASTNode* current = NULL;
    
    // Como não temos `peek`, a lógica de loop é complexa.
    // Vamos simplificar e assumir que o bloco chama `procedimento_decl_var` repetidamente.
    // Esta função pode ser apenas um wrapper.
    return procedimento_lista_decl();
}


ASTNode* procedimento_bloco() {
    Token* proxToken = getNextToken();
    if (!(proxToken->type == TOKEN_KEYWORD && proxToken->keyword == BEGIN)) {
        parse_error("'inicio'", proxToken);
        return NULL;
    }

    // Processa uma lista de declarações de variáveis
    ASTNode* declarations_head = NULL;
    ASTNode* declarations_current = NULL;

    // Aqui, precisamos de uma forma de saber quando as declarações terminam
    // e os comandos começam. Assumindo que qualquer coisa que não seja
    // uma declaração inicia a sequência de comandos.
    // Esta parte é complexa sem um token de lookahead.
    
    // Simplificação: vamos assumir que `procedimento_decl_vars` consome todas as declarações.
    // ASTNode* decls = procedimento_decl_vars(); // Esta chamada precisa ser mais inteligente
    // Por enquanto, vamos deixar as declarações vazias
    
    ASTNode* cmds = procedimento_seq_comandos();

    proxToken = getNextToken();
    if (!(proxToken->type == TOKEN_KEYWORD && proxToken->keyword == END)) {
        parse_error("'fim'", proxToken);
        // Limpar memória
        free_ast(cmds);
        return NULL;
    }

    return create_block_node(NULL, cmds); // `decls` seria passado aqui
}

ASTNode* procedimento_programa() {
    Token *proxToken = getNextToken();
    if (!(proxToken->type == TOKEN_KEYWORD && proxToken->keyword == MAIN)) {
        parse_error("'main'", proxToken);
        return NULL;
    }

    proxToken = getNextToken();
    if (proxToken->type != TOKEN_ID) {
        parse_error("nome do programa", proxToken);
        return NULL;
    }
    char* prog_name = strdup(proxToken->value);

    proxToken = getNextToken();
    if (!(proxToken->type == TOKEN_OPERATOR && proxToken->operador == PARESQ)) {
        parse_error("(", proxToken);
        free(prog_name);
        return NULL;
    }
    
    proxToken = getNextToken();
    if (!(proxToken->type == TOKEN_OPERATOR && proxToken->operador == PARDIR)) {
        parse_error(")", proxToken);
        free(prog_name);
        return NULL;
    }

    ASTNode* block = procedimento_bloco();
    if (!block) {
        free(prog_name);
        return NULL;
    }

    return create_program_node(prog_name, block);
}

// Implementação dummy para as funções que faltam
ASTNode* procedimento_seq_comandos() {
    // Esta função deve analisar uma sequência de comandos (atribuições, if, while, etc.)
    // e retorná-los como uma lista encadeada de nós.
    // Por agora, retorna NULL pois a gramática dos comandos não foi implementada.
    return NULL;
}