#include "env.h"

#include <c_types.h>
#include <stdio.h>

list   sym_map; // 符号表
list   var_map; // 变量表
uint32 var_idx;
stack  section_stack;

void ENV_init(){
    var_map = list_create(var);
    list_expand(&var_map, 1024*4);
    var null_var = VAR(1, 0);
    for(int i = 0; i < 1024*4; i++){
        list_append(&var_map, null_var);
    }

    sym_map = list_create(str_view);
    list_expand(&sym_map, 1024*4);
    str_view null_str = {NULL, 0};
    for(int i = 0; i < 1024*4; i++){
        list_append(&sym_map, null_str);
    }

    var_idx = 0;

    section_stack = stack_create(uint32);
    stack_push(&section_stack, 0);
}


void ENV_push(){
    stack_push(&section_stack, var_idx);
}


void ENV_pop(){
    var_idx = stack_pop(uint32, &section_stack);
}


int ENV_set_var(str_view name){
    list_set(&sym_map, var_idx, name);
    var_idx++;
    return var_idx - 1;
}


int ENV_get_var(str_view name){
    for(int i = var_idx - 1; i >= 0; i--){
        str_view get_name = list_get(str_view, &sym_map, i);
        if(compare_str_view(get_name, name) == 0){
            return i;
        }
    }
    // error: variable not found
    return -1;
}


bool ENV_has_var(str_view name){
    uint32 local_start = stack_top(uint32, &section_stack);
    for(uint32 i = var_idx; i >= local_start; i--){
        str_view get_name = list_get(str_view, &sym_map, i);
        if(compare_str_view(get_name, name) == 0){
            return true;
        }
    }
    return false;
}


struct __ENV__ env = {
    .init = ENV_init,
    .push = ENV_push,
    .pop = ENV_pop,
    .set_var = ENV_set_var,
    .get_var = ENV_get_var,
    .has_var = ENV_has_var
};