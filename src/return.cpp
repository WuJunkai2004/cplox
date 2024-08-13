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
    return_data res = { token(token_type::NIL, "", "", -1), false };
    rets.push_back(res);
    current++;
}


token vm_stack::view_scope(){
    if(current < 0){
        lox::raise_runtime_error(-1, "return statement outside of function");
        return token(token_type::NIL, "", "", -1);
    }
    return rets[current].value;
}


token vm_stack::exit_scope(){
    token res = rets[current].value;
    rets.pop_back();
    current--;
    return res;
}

void vm_stack::set(){
    if(current < 0){
        lox::raise_runtime_error(-1, "return statement outside of function");
        return;
    }
    rets[current].value = token(token_type::NIL, "", "", -1);
    rets[current].is_set = true;
}

void vm_stack::set(token t){
    if(current < 0){
        lox::raise_runtime_error(-1, "return statement outside of function");
        return;
    }
    rets[current].value = t;
    rets[current].is_set = true;
}

token vm_stack::top(){
    return view_scope();
}

token vm_stack::pop(){
    return exit_scope();
}

void vm_stack::push(token t){
    into_scope();
    set(t);
}


bool vm_stack::has_set(){
    if(current < 0){
        return false;
    }
    return rets[current].is_set;
}