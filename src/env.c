#include "env.h"

#include <c_types.h>
#include <stdio.h>

#include "var.h"

list   var_map;
uint32 var_idx;
stack  section_stack;

void ENV_init(){
    var_map = list_create(hash_var);
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


int ENV_set_var(str_view name, uint16 deepth, uint32 offset){
    hash_var v = {
        .name = name,
        .data = VAR(deepth, offset)
    };
    if(var_map.length > var_idx){
        list_set(&var_map, var_idx, v);
    } else {
        list_append(&var_map, v);
    }
    var_idx++;
    return var_idx - 1;
}


var ENV_get_var(str_view name){
    for(uint32 i = var_idx - 1; i > 0; i--){
        hash_var v = list_get(hash_var, &var_map, i - 1);
        if(compare_str_view(v.name, name) == 0){
            return v.data;
        }
    }
    return VAR(0, 0);
}


bool ENV_has_var(str_view name){
    uint32 local_start = stack_top(uint32, &section_stack);
    for(uint32 i = var_idx; i >= local_start; i--){
        hash_var v = list_get(hash_var, &var_map, i);
        if(compare_str_view(v.name, name) == 0){
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