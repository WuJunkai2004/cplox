#include "var.hpp"
#include "token_type.hpp"
#include "token.hpp"
#include "stmt.hpp"

#include <string>
#include <vector>

/**
 * @brief 开销较小的储存变量的类
*/
var::var():
    type(token_type::NIL),
    value("")
{}

var::var(token_type type_, std::string value_):
    type(type_),
    value(value_)
{}

token_type var::get_type(){
    return type;
}

std::string var::get_value(){
    return value;
}


/**
 * @brief 函数类
*/
func::func():
    defined(-1),
    params(),
    body(nullptr)
{}

func::func(std::vector<token> params_, stmt body_):
    defined(0),
    params(params_),
    body(body_)
{}

func::func(std::vector<token> params_, int defined_):
    defined(defined_),
    params(params_),
    body(nullptr)
{}

int func::get_arity(){
    return params.size();
}

stmt func::get_body(){
    return body;
}

std::string func::get_param(int index){
    return params[index].get_lexeme();
}

int func::get_defined(){
    return defined;
}

bool func::is_defined(){
    return defined != -1;
}
