#include "vm.h"

#include <c_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "var.h"
#include "env.h"

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
                    stack_push(&value_stack, CONST_NIL);
                    printf("Error: cannot add");
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
                    stack_push(&value_stack, CONST_NIL);
                    printf("Error: cannot sub");
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
                    stack_push(&value_stack, CONST_NIL);
                    printf("Error: cannot mul");
                }
                ip += 1;
                break;
            }
            case OP_DIV:{
                void* b = LOCALIZE(stack_pop(var, &value_stack));
                void* a = LOCALIZE(stack_pop(var, &value_stack));
                if(GET_TYPE(a) == VAL_NUMBER && GET_TYPE(b) == VAL_NUMBER){
                    double result = AS_NUMBER(a) / AS_NUMBER(b);
                    stack_push(&value_stack, SAVED_NUMBER(RUNTIME_MEMORY, result));
                } else {
                    stack_push(&value_stack, CONST_NIL);
                    printf("Error: cannot div");
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
                        stack_push(&value_stack, CONST_NIL);
                        printf("Error: cannot negate");
                }
                ip += 1;
                break;
            }
            case OP_SET_VAR:{
                ip += 1;
                uint16 pos = *(uint16*)(bytecode + ip);
                hash_var* iter = list_get_ptr(&var_map, pos);
                printf("Set var: %.*s\n", iter->name.len, iter->name.start);
                var value = stack_pop(var, &value_stack);
                printf("Value: %s\n", VAL_FORMAT(LOCALIZE(value)));
                iter->data.deepth = value.deepth;
                iter->data.offset = value.offset;
                ip += 2;
                break;
            }
        }
    }
    if(stack_is_empty(&value_stack)){
        printf("Result: nil\n");
        return 0;
    }
    printf("Result: %s\n", VAL_FORMAT(LOCALIZE(stack_pop(var, &value_stack))));
}


struct __VM__ vm = {
    .init = VM_init,
    .run = VM_run
};