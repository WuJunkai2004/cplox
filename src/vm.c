#include "vm.h"

#include <c_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"

stack value_stack;

void VM_init(){
    value_stack = stack_create(void*);
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
                stack_push(&value_stack, memory.get(deepth, pos));
                ip+=2;
                break;
            }
            case OP_ADD:{
                void* b = stack_pop(void*, &value_stack);
                void* a = stack_pop(void*, &value_stack);
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
                void* b = stack_pop(void*, &value_stack);
                void* a = stack_pop(void*, &value_stack);
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
                void* b = stack_pop(void*, &value_stack);
                void* a = stack_pop(void*, &value_stack);
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
                void* b = stack_pop(void*, &value_stack);
                void* a = stack_pop(void*, &value_stack);
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
        }
    }
    printf("Result: %s\n", VAL_FORMAT(stack_pop(void*, &value_stack)));
}


struct __VM__ vm = {
    .init = VM_init,
    .run = VM_run
};