#include "return.hpp"

#include <vector>
#include "token.hpp"
#include "token_type.hpp"
#include "error.hpp"

return_stack::return_stack():
    current(-1),
    rets()
{}


void return_stack::into_scope(){
    return_data res = { token(token_type::NIL, "", "", -1), false };
    rets.push_back(res);
    current++;
}


token return_stack::exit_scope(){
    token res = rets[current].value;
    rets.pop_back();
    current--;
    return res;
}

void return_stack::set(){
    if(current < 0){
        throw runtime_error("break statement outside of loop");
    }
    rets[current].value = token(token_type::NIL, "", "", -1);
    rets[current].is_set = true;
}

void return_stack::set(token t){
    if(current < 0){
        throw runtime_error("return statement outside of function");
    }
    rets[current].value = t;
    rets[current].is_set = true;
}


bool return_stack::has_set(){
    if(current < 0){
        return false;
    }
    return rets[current].is_set;
}