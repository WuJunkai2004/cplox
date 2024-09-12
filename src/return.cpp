#include "return.hpp"

#include <vector>
#include "token.hpp"
#include "token_type.hpp"
#include "error.hpp"

vm_stack::vm_stack():
    current(-1),
    rets()
{}


void vm_stack::into_scope(){
    return_data res = { var(), false };
    rets.push_back(res);
    current++;
}


var vm_stack::view_scope(){
    if(current < 0){
        lox::raise_runtime_error(-1, "return statement outside of function");
        return var();
    }
    return rets[current].value;
}


var vm_stack::exit_scope(){
    var res = rets[current].value;
    rets.pop_back();
    current--;
    return res;
}

void vm_stack::set(){
    if(current < 0){
        lox::raise_runtime_error(-1, "return statement outside of function");
        return;
    }
    rets[current].value = var();
    rets[current].is_set = true;
}

void vm_stack::set(var t){
    if(current < 0){
        lox::raise_runtime_error(-1, "return statement outside of function");
        return;
    }
    rets[current].value = t;
    rets[current].is_set = true;
}

var vm_stack::top(){
    return view_scope();
}

var vm_stack::pop(){
    return exit_scope();
}

void vm_stack::push(var t){
    into_scope();
    set(t);
}


bool vm_stack::has_set(){
    if(current < 0){
        return false;
    }
    return rets[current].is_set;
}