#ifndef _LOX_NATIVE_C_
#define _LOX_NATIVE_C_

#include "lox_native.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <unistd.h>

struct MEMORY global;
struct MEMORY runtime;

void lox_init(){
    global.data = (byte*)malloc(1024);
    global.idx = 0;
    global.cap = 1024;
    global.max = 0;

    runtime.data = (byte*)malloc(RUNTIME_MEMORY_CAPACITY);
    runtime.idx = 0;
    runtime.cap = RUNTIME_MEMORY_CAPACITY;
    runtime.max = RUNTIME_MEMORY_CAPACITY;

    SAVE_CONST_GLOBAL(VAL_NIL);
    SAVE_CONST_GLOBAL(VAL_FALSE);
    SAVE_CONST_GLOBAL(VAL_TRUE);
}

var SAVE_CONST_GLOBAL(byte value){
    if(global.idx + 1 > global.cap){
        global.data = (byte*)realloc(global.data, global.cap + 1024);
        global.cap += 1024;
    }
    int index = global.idx;
    *((byte*)(global.data + global.idx)) = value;
    global.idx += 1;
    return index;
}

var SAVE_NUMBER_GLOBAL(number value){
    if(global.idx + sizeof(number) + 1 > global.cap){
        global.data = (byte*)realloc(global.data, global.cap + 1024);
        global.cap += 1024;
    }
    int index = global.idx;
    *((byte*)  (global.data + global.idx)) = VAL_NUMBER;
    *((number*)(global.data + global.idx + 1)) = value;
    global.idx += sizeof(number) + 1;
    return index;
}

var SAVE_STRING_GLOBAL(string value){
    int len = strlen(value);
    if(global.idx + len + 2 > global.cap){
        global.data = (byte*)realloc(global.data, global.cap + 1024);
        global.cap += 1024;
    }
    int index = global.idx;
    *((byte*)(global.data + global.idx)) = VAL_STRING;
    memcpy(global.data + global.idx + 1, value, len + 1);
    global.idx += len + 2;
    return index;
}

var SAVE_FUNC_GLOBAL(func value){
    if(global.idx + sizeof(func) + 1 > global.cap){
        global.data = (byte*)realloc(global.data, global.cap + 1024);
        global.cap += 1024;
    }
    int index = global.idx;
    *((byte*)(global.data + global.idx)) = VAL_FUNC;
    *((func*)(global.data + global.idx + 1)) = value;
    global.idx += sizeof(func) + 1;
    return index;
}

var SAVE_NUMBER(number value){
    int index = allocate(sizeof(number) + 1);
    *((byte*)  (runtime.data + index)) = VAL_NUMBER;
    *((number*)(runtime.data + index + 1)) = value;
    return index + RUNTIME_MARK;
}

var SAVE_STRING(string value){
    int len = strlen(value);
    int index = allocate(len + 2);
    *((byte*)(runtime.data + index)) = VAL_STRING;
    memcpy(runtime.data + index + 1, value, len + 1);
    return index + RUNTIME_MARK;
}

var SAVE_FUNC(func value){
    int index = allocate(sizeof(func) + 1);
    *((byte*)(runtime.data + index)) = VAL_FUNC;
    *((func*)(runtime.data + index + 1)) = value;
    return index + RUNTIME_MARK;
}

int allocate(int size){
    if(runtime.idx + size > runtime.max){
        garbage_collect();
    }
    int meet_data = -1;
    for(int idx=runtime.idx; idx<runtime.idx+size; idx++){
        if(*(char*)(runtime.data + idx)){
            meet_data = idx;
            break;
        }
    }
    if(meet_data == -1){
        int index = runtime.idx;
        runtime.idx += size;
        return index;
    }
    int type = GET_TYPE_PTR(runtime.data + meet_data);
    switch(type){
        case VAL_NUMBER:
            runtime.idx += sizeof(number) + 1;
            break;
        case VAL_STRING:
            runtime.idx += strlen(AS_STRING_PTR(runtime.data + index)) + 2;
            break;
        case VAL_FUNC:
            runtime.idx += sizeof(func) + 1;
            break;
    }
    return allocate(size);
}

var adds(var a, var b){
    if(GET_TYPE(a) == VAL_NUMBER && GET_TYPE(b) == VAL_NUMBER){
        return SAVE_NUMBER(AS_NUMBER(a) + AS_NUMBER(b));
    }
    if(GET_TYPE(a) == VAL_STRING && GET_TYPE(b) == VAL_STRING){
        int len_a = strlen(AS_STRING(a));
        int len_b = strlen(AS_STRING(b));
        string new_str = (string)malloc(len_a + len_b + 1);
        memcpy(new_str, AS_STRING(a), len_a);
        memcpy(new_str + len_a, AS_STRING(b), len_b + 1);
        int ret = SAVE_STRING(new_str);
        free(new_str);
        return ret;
    }
    raise("TypeError", "Unsupported operand types for +");
}

var subs(var a, var b){
    if(GET_TYPE(a) == VAL_NUMBER && GET_TYPE(b) == VAL_NUMBER){
        return SAVE_NUMBER(AS_NUMBER(a) - AS_NUMBER(b));
    }
    raise("TypeError", "Unsupported operand types for -");
}

var muls(var a, var b){
    if(GET_TYPE(a) == VAL_NUMBER && GET_TYPE(b) == VAL_NUMBER){
        return SAVE_NUMBER(AS_NUMBER(a) * AS_NUMBER(b));
    }
    raise("TypeError", "Unsupported operand types for *");
}

var divs(var a, var b){
    if(GET_TYPE(a) != VAL_NUMBER || GET_TYPE(b) != VAL_NUMBER){
        raise("TypeError", "Unsupported operand types for /");
    }
    number divisor = AS_NUMBER(b);
    if(divisor == 0){
        raise("ZeroDivisionError", "division by zero");
    }
    return SAVE_NUMBER(AS_NUMBER(a) / divisor);
}

var negates(var a){
    switch(GET_TYPE(a)){
        case VAL_NUMBER:
            return SAVE_NUMBER(-AS_NUMBER(a));
        case VAL_NIL:
        case VAL_FALSE:
            return VAL_TRUE;
        case VAL_TRUE:
            return VAL_FALSE;
    }
    raise("TypeError", "Unsupported operand type for negate");
}

var eqs(var a, var b){
    if(IS_BOOL(a) && IS_BOOL(b)){
        return a == b ? VAL_TRUE : VAL_FALSE;
    }
    if(GET_TYPE(a) != GET_TYPE(b)){
        raise("TypeError", "Cannot compare different types");
    }
    switch(GET_TYPE(a)){
        case VAL_NUMBER:
            return AS_NUMBER(a) == AS_NUMBER(b) ? VAL_TRUE : VAL_FALSE;
        case VAL_STRING:
            return strcmp(AS_STRING(a), AS_STRING(b)) == 0 ? VAL_TRUE : VAL_FALSE;
        default:
            return a == b ? VAL_TRUE : VAL_FALSE;
    }
}

var neqs(var a, var b){
    return eqs(a, b) == VAL_TRUE ? VAL_FALSE : VAL_TRUE;
}

var lts(var a, var b){
    if(GET_TYPE(a) != GET_TYPE(b)){
        raise("TypeError", "Cannot compare different types");
    }
    switch(GET_TYPE(a)){
        case VAL_NUMBER:
            return AS_NUMBER(a) < AS_NUMBER(b) ? VAL_TRUE : VAL_FALSE;
        case VAL_STRING:
            return strcmp(AS_STRING(a), AS_STRING(b)) < 0 ? VAL_TRUE : VAL_FALSE;
        default:
            raise("TypeError", "Unsupported operand types for <");
    }
}

var gts(var a, var b){
    if(GET_TYPE(a) != GET_TYPE(b)){
        raise("TypeError", "Cannot compare different types");
    }
    switch(GET_TYPE(a)){
        case VAL_NUMBER:
            return AS_NUMBER(a) > AS_NUMBER(b) ? VAL_TRUE : VAL_FALSE;
        case VAL_STRING:
            return strcmp(AS_STRING(a), AS_STRING(b)) > 0 ? VAL_TRUE : VAL_FALSE;
        default:
            raise("TypeError", "Unsupported operand types for >");
    }
}

var les(var a, var b){
    return gts(a, b) == VAL_TRUE ? VAL_FALSE : VAL_TRUE;
}

var ges(var a, var b){
    return lts(a, b) == VAL_TRUE ? VAL_FALSE : VAL_TRUE;
}

var assign(var origin, var value){
    int refer_count = (origin & REFER_SECTION) >> 5;
    if(refer_count){
        if(refer_count == 1){
            MASK_AS_GARBAGE(origin);
        }else{
            REFER_SUB(origin);
        }
    }
    REFER_ADD(value);
    return value;
}

int is_true(var a){
    switch(GET_TYPE(a)){
        case VAL_NIL:
        case VAL_FALSE:
            return 0;
        case VAL_NUMBER:
            return AS_NUMBER(a) == 0 ? 0 : 1;
        case VAL_STRING:
            return AS_STRING(a)[0] == '\0' ? 0 : 1;
        default:
            return 1;
    }
}


void garbage_collect(){
    printf("in gc\n");
    int   end = runtime.idx;
    runtime.idx  = 0;
    for(int cur=0; cur<end;){
        byte type = GET_TYPE_PTR(runtime.data + cur);
        int is_garbage = IS_GARBAGE_PTR(runtime.data + cur);
        int size = 0;
        switch(type){
            case VAL_STRING:
                size = strlen(AS_STRING_PTR(runtime.data + cur)) + 2;
                break;
            case VAL_FUNC:
                size = sizeof(func) + 1;
                break;
            default:
                size = sizeof(number) + 1;
        }
        if(is_garbage){
            for(int i=cur; i<cur+size; i++){
                *(runtime.data + i) = (byte)0;
            }
        }
        cur += size;
    }
}


void raise(string type, string msg){
    printf("[%s]:\n", type);
    printf("  %s\n", msg);
    exit(1);
}


string FORMAT(int a){
    static char buffer[1024];
    switch(GET_TYPE(a)){
        case VAL_NIL:
            return "nil";
        case VAL_TRUE:
            return "true";
        case VAL_FALSE:
            return "false";
        case VAL_NUMBER:
            sprintf(buffer, "%lf", AS_NUMBER(a));
            return buffer;
        case VAL_STRING:
            return AS_STRING(a);
        case VAL_FUNC:
            return "<func>";
        case VAL_CLASS:
            return "<class>";
        case VAL_METHOD:
            return "<method>";
    }
    return "unknown";
}


/* NATIVE FUNCTION AND CLASS*/ 
int __print(int a){
    printf(FORMAT(a));
    printf("\n");
    return NATIVE_NIL;
}


int __clock(){
    return SAVE_NUMBER((number)clock() / CLOCKS_PER_SEC);
}


int __input(int a){
    char buffer[1024];
    fgets(buffer, 1024, stdin);
    buffer[strlen(buffer) - 1] = '\0';
    return SAVE_STRING(buffer);
}


int __sleep(int a){
    sleep(AS_NUMBER(a));
    return NATIVE_NIL;
}


int __exit(int a){
    exit(AS_NUMBER(a));
    return NATIVE_NIL;
}


#endif