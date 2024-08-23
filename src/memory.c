#include "memory.h"

#include <c_types.h>
#include <string.h>
#include <stdio.h>

list mem = {};

struct __MEMORY__ memory = {
    .init = MEMORY_init,
    .save_const = MEMORY_save_const,
    .save_number = MEMORY_save_number,
    .save_string = MEMORY_save_string,
    .get = MEMORY_get
};


int MEMORY_save_const(int deepth, uint8  value){
    list* local = list_get_ptr(&mem, deepth);
    if(local->length + 1 > local->_capacity){
        list_expand(local, local->_capacity + 1024 * 8);
    }
    int index = local->length;
    local->length += 1;
    *(uint8*)(local->_data + index) = value;
    return index;
}


int MEMORY_save_number(int deepth, double value){
    list* local = list_get_ptr(&mem, deepth);
    if(local->length + sizeof(double) + 1 > local->_capacity){
        list_expand(local, local->_capacity + 1024 * 8);
    }
    int index = local->length;
    local->length += sizeof(double) + 1;
    *(uint8*) (local->_data + index) = VAL_NUMBER;
    *(double*)(local->_data + index + 1) = value;
    return index;
}


int MEMORY_save_string(int deepth, str value){
    list* local = list_get_ptr(&mem, deepth);
    int len = strlen(value);
    if(local->length + len + 2 > local->_capacity){
        list_expand(local, local->_capacity + 1024 * 8);
    }
    int index = local->length;
    local->length += len + 2;
    *(uint8*) (local->_data + index) = VAL_STRING;
    memcpy(local->_data + index + 1, value, len + 1);
    return index;
}


void* MEMORY_get(int deepth, int index){
    return list_get(list, &mem, deepth)._data + index;
}


void MEMORY_init(void){
    mem = list_create(list);
    list_append(&mem, list_create(uint8));
    list_append(&mem, list_create(uint8));

    memory.save_const(STATIC_MEMORY, VAL_NIL);
    memory.save_const(STATIC_MEMORY, VAL_FALSE);
    memory.save_const(STATIC_MEMORY, VAL_TRUE);
}


str VAL_FORMAT(void* ptr){
    static str format_nil = "nil";
    static str format_false = "false";
    static str format_true = "true";
    switch(GET_TYPE(ptr)){
        case VAL_NIL:    return format_nil;
        case VAL_FALSE:  return format_false;
        case VAL_TRUE:   return format_true;
        case VAL_NUMBER: return string(AS_NUMBER(ptr));
        case VAL_STRING: return AS_STRING(ptr);
        default: return "unknown";
    }
}