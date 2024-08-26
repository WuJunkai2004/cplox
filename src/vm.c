#include "vm.h"

#include <c_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "env.h"
#include "error.h"

stack value_stack;

void VM_init(){
    value_stack = stack_create(var);
}

int VM_run(chuck bytecode, uint32 length){
    int ip = 0;
    while (ip < length){
        switch(bytecode[ip]){
            case OP_CONSTANT:{
                ip+=1;
                uint8 deepth = bytecode[ip];
                ip+=1;
                uint16 pos = *(uint16*)(bytecode + ip);
                stack_push(&value_stack, VAR(deepth, pos));
                ip+=2;
                break;
            }
            case OP_ADD:{
                void* b = LOCALIZE(stack_pop(var, &value_stack));
                void* a = LOCALIZE(stack_pop(var, &value_stack));
                if(GET_TYPE(a) == VAL_NUMBER && GET_TYPE(b) == VAL_NUMBER){
                    double result = AS_NUMBER(a) + AS_NUMBER(b);
                    stack_push(&value_stack, SAVED_NUMBER(RUNTIME_MEMORY, result));
                } else if(GET_TYPE(a) == VAL_STRING && GET_TYPE(b) == VAL_STRING){
                    str a_str = AS_STRING(a);
                    str b_str = AS_STRING(b);
                    int len = strlen(a_str) + strlen(b_str) + 2;
                    char result[len] = {};
                    sprintf(result, "%s%s\0", a_str, b_str);
                    stack_push(&value_stack, SAVED_STRING(RUNTIME_MEMORY, result));
                } else {
                    throw(TYPE_ERROR, "cannot add '%s' and '%s'", VAL_FORMAT(a), VAL_FORMAT(b));
                }
                ip += 1;
                break;
            }
            case OP_SUB:{
                void* b = LOCALIZE(stack_pop(var, &value_stack));
                void* a = LOCALIZE(stack_pop(var, &value_stack));
                if(GET_TYPE(a) == VAL_NUMBER && GET_TYPE(b) == VAL_NUMBER){
                    double result = AS_NUMBER(a) - AS_NUMBER(b);
                    stack_push(&value_stack, SAVED_NUMBER(RUNTIME_MEMORY, result));
                } else {
                    throw(TYPE_ERROR, "cannot sub '%s' and '%s'", VAL_FORMAT(a), VAL_FORMAT(b));
                }
                ip += 1;
                break;
            }
            case OP_MUL:{
                void* b = LOCALIZE(stack_pop(var, &value_stack));
                void* a = LOCALIZE(stack_pop(var, &value_stack));
                if(GET_TYPE(a) == VAL_NUMBER && GET_TYPE(b) == VAL_NUMBER){
                    double result = AS_NUMBER(a) * AS_NUMBER(b);
                    stack_push(&value_stack, SAVED_NUMBER(RUNTIME_MEMORY, result));
                } else {
                    throw(TYPE_ERROR, "cannot mul '%s' and '%s'", VAL_FORMAT(a), VAL_FORMAT(b));
                }
                ip += 1;
                break;
            }
            case OP_DIV:{
                void* b = LOCALIZE(stack_pop(var, &value_stack));
                void* a = LOCALIZE(stack_pop(var, &value_stack));
                if(GET_TYPE(a) == VAL_NUMBER && GET_TYPE(b) == VAL_NUMBER){
                    if(AS_NUMBER(b) == 0){
                        throw(ZERO_DIVISION_ERROR, "division by zero");
                    }
                    double result = AS_NUMBER(a) / AS_NUMBER(b);
                    stack_push(&value_stack, SAVED_NUMBER(RUNTIME_MEMORY, result));
                } else {
                    throw(TYPE_ERROR, "cannot div '%s' and '%s'", VAL_FORMAT(a), VAL_FORMAT(b));
                }
                ip += 1;
                break;
            }
            case OP_NEGATE:{
                void* a = LOCALIZE(stack_pop(var, &value_stack));
                switch(GET_TYPE(a)){
                    case VAL_NUMBER:
                        stack_push(&value_stack, SAVED_NUMBER(RUNTIME_MEMORY, -AS_NUMBER(a)));
                        break;
                    case VAL_TRUE:
                        stack_push(&value_stack, CONST_FALSE);
                        break;
                    case VAL_FALSE:
                    case VAL_NIL:
                        stack_push(&value_stack, CONST_TRUE);
                        break;
                    default:
                        throw(TYPE_ERROR, "cannot negate '%s'", VAL_FORMAT(a));
                        break;
                }
                ip += 1;
                break;
            }
            case OP_SET_GLOBAL:{
                ip += 1;
                uint16 pos = *(uint16*)(bytecode + ip);
                var* iter     = list_get_ptr(&var_map, pos);
                str_view name = list_get(str_view, &sym_map, pos);
                printf("Set var: %.*s\n", name.len, name.start);
                var value = stack_top(var, &value_stack);
                printf("Value: %s\n", VAL_FORMAT(LOCALIZE(value)));
                iter->deepth = value.deepth;
                iter->offset = value.offset;
                ip += 2;
                break;
            }
            case OP_GET_GLOBAL:{
                ip += 1;
                uint16 pos = *(uint16*)(bytecode + ip);
                var iter = list_get(var, &var_map, pos);
                stack_push(&value_stack, iter);
                ip += 2;
                break;
            }
            case OP_END_STMT:
                stack_clear(&value_stack);
                ip += 1;
                break;
        }
    }
    if(stack_is_empty(&value_stack)){
        printf("Result: nil\n");
        return 0;
    }
    printf("Result: %s\n", VAL_FORMAT(LOCALIZE(stack_pop(var, &value_stack))));
    while(!stack_is_empty(&value_stack)){
        stack_pop(var, &value_stack);
    }
}


struct __VM__ vm = {
    .init = VM_init,
    .run = VM_run
};