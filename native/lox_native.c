#ifndef _LOX_NATIVE_C_
#define _LOX_NATIVE_C_

#include "lox_native.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct MEMORY global;
struct MEMORY runtime;

void lox_init(){
    global.data = (byte*)malloc(1024);
    global.idx = 0;
    global.cap = 1024;
    global.max = 0;

    runtime.data = (byte*)malloc(1024);
    runtime.idx = 0;
    runtime.cap = 1024;
    runtime.max = 1024 * 16;

    SAVE_CONST_GLOBAL(VAL_NIL);
    SAVE_CONST_GLOBAL(VAL_FALSE);
    SAVE_CONST_GLOBAL(VAL_TRUE);
}

int SAVE_CONST_GLOBAL(byte value){
    if(global.idx + 1 > global.cap){
        global.data = (byte*)realloc(global.data, global.cap + 1024);
        global.cap += 1024;
    }
    int index = global.idx;
    *((byte*)(global.data + global.idx)) = value;
    global.idx += 1;
    return index;
}

int SAVE_NUMBER_GLOBAL(number value){
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

int SAVE_STRING_GLOBAL(string value){
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

int SAVE_FUNC_GLOBAL(func value){
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

int SAVE_CONST(byte value){
    if(runtime.idx + 1 > runtime.cap){
        runtime.data = (byte*)realloc(runtime.data, runtime.cap + 1024);
        runtime.cap += 1024;
    }
    int index = runtime.idx;
    *((byte*)(runtime.data + runtime.idx)) = value;
    runtime.idx += 1;
    return index + RUNTIME_MARK;
}

int SAVE_NUMBER(number value){
    if(runtime.idx + sizeof(number) + 1 > runtime.cap){
        runtime.data = (byte*)realloc(runtime.data, runtime.cap + 1024);
        runtime.cap += 1024;
    }
    int index = runtime.idx;
    *((byte*)  (runtime.data + runtime.idx)) = VAL_NUMBER;
    *((number*)(runtime.data + runtime.idx + 1)) = value;
    runtime.idx += sizeof(number) + 1;
    return index + RUNTIME_MARK;
}

int SAVE_STRING(string value){
    int len = strlen(value);
    if(runtime.idx + len + 2 > runtime.cap){
        runtime.data = (byte*)realloc(runtime.data, runtime.cap + 1024);
        runtime.cap += 1024;
    }
    int index = runtime.idx;
    *((byte*)(runtime.data + runtime.idx)) = VAL_STRING;
    memcpy(runtime.data + runtime.idx + 1, value, len + 1);
    runtime.idx += len + 2;
    return index + RUNTIME_MARK;
}

int SAVE_FUNC(func value){
    if(runtime.idx + sizeof(func) + 1 > runtime.cap){
        runtime.data = (byte*)realloc(runtime.data, runtime.cap + 1024);
        runtime.cap += 1024;
    }
    int index = runtime.idx;
    *((byte*)(runtime.data + runtime.idx)) = VAL_FUNC;
    *((func*)(runtime.data + runtime.idx + 1)) = value;
    runtime.idx += sizeof(func) + 1;
    return index + RUNTIME_MARK;
}

int adds(int a, int b){
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

int subs(int a, int b){
    if(GET_TYPE(a) == VAL_NUMBER && GET_TYPE(b) == VAL_NUMBER){
        return SAVE_NUMBER(AS_NUMBER(a) - AS_NUMBER(b));
    }
    raise("TypeError", "Unsupported operand types for -");
}

int muls(int a, int b){
    if(GET_TYPE(a) == VAL_NUMBER && GET_TYPE(b) == VAL_NUMBER){
        return SAVE_NUMBER(AS_NUMBER(a) * AS_NUMBER(b));
    }
    raise("TypeError", "Unsupported operand types for *");
}

int divs(int a, int b){
    if(GET_TYPE(a) != VAL_NUMBER || GET_TYPE(b) != VAL_NUMBER){
        raise("TypeError", "Unsupported operand types for /");
    }
    number divisor = AS_NUMBER(b);
    if(divisor == 0){
        raise("ZeroDivisionError", "division by zero");
    }
    return SAVE_NUMBER(AS_NUMBER(a) / divisor);
}

int negates(int a){
    switch(GET_TYPE(a)){
        case VAL_NUMBER:
            return SAVE_NUMBER(-AS_NUMBER(a));
        case VAL_NIL:
        case VAL_FALSE:
            return 2;
        case VAL_TRUE:
            return 1;
    }
    raise("TypeError", "Unsupported operand type for negate");
}

void raise(string type, string msg){
    printf("[%s]:\n\t%s\n", type, msg);
    exit(1);
}



#endif