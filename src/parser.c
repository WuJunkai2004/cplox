#include "parser.h"

#include <c_types.h>
#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "vm.h"
#include "memory.h"
#include "env.h"
#include "error.h"
#include "file.h"

struct __PARSER__ parser = {
    .parse = PARSER_parse,
};

static uint8 brace_deepth = 0;

static void EMIT_GLOBAL_VAR(code* result, str_view var_name){
    str var_name_str = str_new(var_name.len + 2);
    var_name_str[0] = '_';
    for(int i = 0; i < var_name.len; i++){
        var_name_str[i + 1] = var_name.start[i];
    }
    var_name_str[var_name.len + 1] = '\0';
    list_append(&result->global_var, var_name_str);
}


void CONSUME(list* token_list, int* idx, enum token_type type, str msg){
    if(list_get(token, token_list, *idx).type != type){
        raise(list_get(token, token_list, *idx).line, SYNTAX_ERROR, msg);
    }
    *idx += 1;
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

static void parse_expr(list* expr, list* tokens, int* idx){
    stack sign_stack = stack_create(token);
    bool is_negate = false;
    while(list_get(token, tokens, *idx).type != TOKEN_SEMICOLON && list_get(token, tokens, *idx).type != TOKEN_COMMA){
        switch(list_get(token, tokens, *idx).type){
            case TOKEN_LEFT_PAREN:
                if(*idx != 0 && list_get(token, tokens, *idx - 1).type == TOKEN_IDENTIFIER){
                    /** @todo */
                } else {
                    stack_push(&sign_stack, list_get(token, tokens, *idx));
                    *idx += 1;
                }
                break;
            case TOKEN_RIGHT_PAREN:
                while(!stack_is_empty(&sign_stack) && stack_top(token, &sign_stack).type != TOKEN_LEFT_PAREN){
                    list_append(&expr, stack_pop(token, &sign_stack));
                }
                stack_pop(token, &sign_stack);
                *idx += 1;
                break;
            case TOKEN_MINUS:
                // 先处理负号，减号 fall through
                if(*idx == 0 || (list_get(token, tokens, *idx - 1).type != TOKEN_NUMBER &&
                                 list_get(token, tokens, *idx - 1).type != TOKEN_RIGHT_PAREN &&
                                 list_get(token, tokens, *idx - 1).type != TOKEN_IDENTIFIER &&
                                 list_get(token, tokens, *idx - 1).type != TOKEN_STRING &&
                                 list_get(token, tokens, *idx - 1).type != TOKEN_NIL &&
                                 list_get(token, tokens, *idx - 1).type != TOKEN_FALSE &&
                                 list_get(token, tokens, *idx - 1).type != TOKEN_TRUE)){
                    *idx += 1;
                    is_negate = !is_negate;
                    continue;
                }
            case TOKEN_PLUS:
            case TOKEN_STAR:
            case TOKEN_SLASH:
                while(!stack_is_empty(&sign_stack) && precedence[stack_top(token, &sign_stack).type] >= precedence[list_get(token, tokens, *idx).type]){
                    list_append(&expr, stack_pop(token, &sign_stack));
                }
                stack_push(&sign_stack, list_get(token, tokens, *idx));
                *idx += 1;
                break;
            case TOKEN_NUMBER:
            case TOKEN_STRING:
            case TOKEN_IDENTIFIER:
            case TOKEN_NIL:
            case TOKEN_FALSE:
            case TOKEN_TRUE:
                list_append(&expr, list_get(token, tokens, *idx));
                *idx += 1;
                break;
            case TOKEN_EQUAL:
                throw(SYNTAX_ERROR, "Unexpected token = in expression");
            case TOKEN_BANG:
                is_negate = !is_negate;
                *idx += 1;
                continue;
            default:
                printf("Error: Unexpected token\n");
                printf("%d\n", *idx);
                printf("token type: %d\n", list_get(token, tokens, *idx).type);
                printf("is equal semicolon %d\n", list_get(token, tokens, *idx).type == TOKEN_SEMICOLON);
                stack_free(&sign_stack);
                return;
        }
        if(is_negate){
            list_append(&expr, (token){.type = TOKEN_MINUS});
            is_negate = false;
        }
    }
    while(!stack_is_empty(&sign_stack)){
        list_append(&expr, stack_pop(token, &sign_stack));
    }
    stack_free(&sign_stack); 
}


static void yield_expr(FILE* buffer, list* expr){
    // generate as adds(1, muls(2, divs(3, 4)), 5)
    for(int i = 0; i < expr->len; i++){
        switch(list_get(char, expr, i)){
            case TOKEN_PLUS:
                fprintf(buffer, "adds(");
                break;
            case TOKEN_MINUS:
                fprintf(buffer, "subs(");
                break;
            case TOKEN_STAR:
                fprintf(buffer, "muls(");
                break;
            case TOKEN_SLASH:
                fprintf(buffer, "divs(");
                break;
            case TOKEN_NUMBER:
                fprintf(buffer, "%s", list_get(token, expr, i).lexeme.start);
                break;
            case TOKEN_STRING:
                fprintf(buffer, "\"%s\"", list_get(token, expr, i).lexeme.start);
                break;
            case TOKEN_IDENTIFIER:
                fprintf(buffer, "%s", list_get(token, expr, i).lexeme.start);
                break;
            case TOKEN_NIL:
                fprintf(buffer, "nil");
                break;
            case TOKEN_FALSE:
                fprintf(buffer, "false");
                break;
            case TOKEN_TRUE:
                fprintf(buffer, "true");
                break;
            default:
                break;
        }
    }
}


static void parse_stmt(code* result, list* tokens, int* idx){
    switch(list_get(token, tokens, *idx).type){
        case TOKEN_VAR:{
            *idx += 1;
            str var_name = str_new(list_get(token, tokens, *idx).lexeme.len + 2);
            sprintf(var_name, "_%.*s", list_get(token, tokens, *idx).lexeme.len, list_get(token, tokens, *idx).lexeme.start);
            list_append(&result->global_var, var_name);
            *idx += 1;
            if(list_get(token, tokens, *idx).type == TOKEN_EQUAL){
                *idx += 1;
                parse_expr(&result->main_code, tokens, idx);
            } else {
                EMIT_OPCODE(bytecode, OP_CONSTANT);
                EMIT_CONST(bytecode, 0);
                EMIT_OPCODE(bytecode, OP_SET_GLOBAL);
                EMIT_OFFSET(bytecode, set_pos);
            }
            break;
        }
        case TOKEN_FUNC:
            break;
        case TOKEN_IF:
            break;
        case TOKEN_FOR:
            break;
        case TOKEN_WHILE:
            break;
        case TOKEN_RETURN:
            break;
        case TOKEN_BREAK:
            break;
        case TOKEN_IMPORT:
            break;
        case TOKEN_EOF:
            *idx += 1;
            break;
        default:
            parse_expr(bytecode, tokens, idx);
            break;
    }
    CONSUME(tokens, idx, TOKEN_SEMICOLON, "Expect ; after expression");
    EMIT_OPCODE(bytecode, OP_END_STMT);
}


static void parse_block(code* result, list* tokens, int* idx){
    brace_deepth += 1;
    while(list_get(token, tokens, *idx).type != TOKEN_RIGHT_BRACE){
        parse_stmt(bytecode, tokens, idx);
    }
    *idx += 1;
    brace_deepth -= 1;
}


/**
 * @brief parse the token list to bytecode
 * @param tokens: the token list
 * @return the bytecode list
*/
code PARSER_parse(list tokens){
    code result = {
        .global_var = list_create(str),
        .main_code =  list_create(char),
    }
    int idx = 0;
    while(list_get(token, &tokens, idx).type != TOKEN_EOF){
        parse_stmt(&code, &tokens, &idx);
    }
    return result;
}