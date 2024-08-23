#include "parser.h"

#include <c_types.h>
#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "vm.h"
#include "memory.h"

struct __PARSER__ parser = {
    .parse = PARSER_parse,
    .products = {}
};

static uint8 brace_deepth = 1;

void EMIT_SIGN(list* bytecode, uint8 sign){
    if(bytecode->length + 1 > bytecode->_capacity){
        list_expand(bytecode, bytecode->_capacity + 1024);
    }
    switch(sign){
        case TOKEN_PLUS:
            *(uint8*)(bytecode->_data + bytecode->length) = OP_ADD;
            break;
        case TOKEN_MINUS:
            *(uint8*)(bytecode->_data + bytecode->length) = OP_SUB;
            break;
        case TOKEN_STAR:
            *(uint8*)(bytecode->_data + bytecode->length) = OP_MUL;
            break;
        case TOKEN_SLASH:
            *(uint8*)(bytecode->_data + bytecode->length) = OP_DIV;
            break;
        default:
            return;
    }
    bytecode->length += 1;
}

void EMIT_OPCODE(list* bytecode, uint8 opcode){
    if(bytecode->length + 1 > bytecode->_capacity){
        list_expand(bytecode, bytecode->_capacity + 1024);
    }
    *(uint8*)(bytecode->_data + bytecode->length) = opcode;
    bytecode->length += 1;
}

void EMIT_VALUE(list* bytecode, uint16 pos){
    if(bytecode->length + 3 > bytecode->_capacity){
        list_expand(bytecode, bytecode->_capacity + 1024);
    }
    *(uint8*)(bytecode->_data + bytecode->length) = brace_deepth;
    *(uint16*)(bytecode->_data + bytecode->length + 1) = pos;
    bytecode->length += 3;
}


static int precedence[32] = {
    [TOKEN_LEFT_PAREN] = 0,
    [TOKEN_RIGHT_PAREN] = 0,
    [TOKEN_DOT] = 6,
    [TOKEN_MINUS] = 4,
    [TOKEN_PLUS] = 4,
    [TOKEN_SLASH] = 5,
    [TOKEN_STAR] = 5,
    [TOKEN_BANG] = 3,
    [TOKEN_BANG_EQUAL] = 2,
    [TOKEN_EQUAL] = 1,
    [TOKEN_EQUAL_EQUAL] = 2,
    [TOKEN_GREATER] = 2,
    [TOKEN_GREATER_EQUAL] = 2,
    [TOKEN_LESS] = 2,
    [TOKEN_LESS_EQUAL] = 2,
    [TOKEN_IDENTIFIER] = 0,
    [TOKEN_STRING] = 0,
    [TOKEN_NUMBER] = 0,
    [TOKEN_TRUE] = 0,
    [TOKEN_FALSE] = 0,
    [TOKEN_NIL] = 0,
    [TOKEN_AND] = 0,
    [TOKEN_OR] = 0
};

static void parse_expr(list* bytecode, list* tokens, int* idx){
    stack sign_stack = stack_create(token);
    while(list_get(token, tokens, *idx).type != TOKEN_EOF){
        switch(list_get(token, tokens, *idx).type){
            case TOKEN_LEFT_PAREN:{
                *idx += 1;
                parse_expr(bytecode, tokens, idx);
                break;
            }
            case TOKEN_RIGHT_PAREN:
                while(!stack_is_empty(&sign_stack)){
                    EMIT_SIGN(bytecode, stack_pop(token, &sign_stack).type);
                }
                *idx += 1;
                stack_free(&sign_stack);
                return;
            case TOKEN_PLUS:
            case TOKEN_MINUS:
            case TOKEN_STAR:
            case TOKEN_SLASH:
                while(!stack_is_empty(&sign_stack) && precedence[stack_top(token, &sign_stack).type] >= precedence[list_get(token, tokens, *idx).type]){
                    EMIT_SIGN(bytecode, stack_pop(token, &sign_stack).type);
                }
                stack_push(&sign_stack, list_get(token, tokens, *idx));
                *idx += 1;
                break;
            case TOKEN_NUMBER:
                EMIT_OPCODE(bytecode, OP_CONSTANT);
                EMIT_VALUE(bytecode, memory.save_number(brace_deepth, strtod(list_get(token, tokens, *idx).lexeme.start, NULL)));
                *idx += 1; 
                break;
            case TOKEN_STRING:
                EMIT_OPCODE(bytecode, OP_CONSTANT);
                EMIT_VALUE(bytecode, memory.save_string(brace_deepth, view_to_str(list_get(token, tokens, *idx).lexeme)));
                *idx += 1;
                break;
            default:
                break;
        }
    }
    while(!stack_is_empty(&sign_stack)){
        EMIT_SIGN(bytecode, stack_pop(token, &sign_stack).type);
    }
    stack_free(&sign_stack);
}


/**
 * @brief parse the token list to bytecode
 * @param tokens: the token list
 * @return the bytecode list
*/
list PARSER_parse(list tokens){
    parser.products = list_create(uint8);
    for(int idx = 0; idx < tokens.length; idx++){
        token local = list_get(token, &tokens, idx);
        switch(local.type){
            case TOKEN_VAR:
                break;
            case TOKEN_FUNC:
                break;
            case TOKEN_IF:
                break;
            default:
                parse_expr(&parser.products, &tokens, &idx);
        }
    }
    return parser.products;
}