#include "env.hpp"
#include "lox.hpp"
#include "token.hpp"
#include "token_type.hpp"

#include <string>
#include <map>

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
    defined(false),
    params(),
    body(nullptr)
{}

func::func(std::vector<token> params_, stmt body_):
    defined(true),
    params(params_),
    body(body_)
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

bool func::is_defined(){
    return defined;
}


/**
 * @brief 变量的环境类
*/
environment::environment(environment* parent_):
    parent(parent_)
{}

environment::~environment(){}

bool environment::exists(std::string name){
    return table.find(name) != table.end();
}

void environment::define(std::string name, var value){
    table[name] = value;
}

void environment::define_func(std::string name, std::vector<token> params, stmt body){
    fun[name] = func(params, body);
}

var environment::get(std::string name){
    if(table.find(name) != table.end()){
        return table[name];
    }
    if(parent != nullptr){
        return parent->get(name);
    }
    lox::error(-1, "Undefined variable '" + name + "'.");
    return var();
}

func environment::get_func(std::string name){
    return fun[name];
}

environment* environment::get_parent(){
    return parent;
}


/**
 * @brief 操作环境的函数
*/
void env::init(){
    static bool had_init = false;
    if(had_init){
        return;
    }
    had_init = true;
    global = new environment();
    locale = global;
}

void env::push(){
    environment* parent = locale;
    locale = new environment(parent);
}

void env::pop(){
    if(locale == global){
        return;
    }
    environment* parent = locale->get_parent();
    delete locale;
    locale = parent;
}


/**
 * @brief 操作环境中变量的函数
*/
void env::define(std::string name, token value, environment* current){
    if(current->exists(name)){
        lox::error(value.get_line(), "Variable '" + name + "' already declared in this scope.");
        return;
    }
    var saved(value.get_type(), value.get_literal());
    current->define(name, saved);
}

void env::assign(std::string name, token value, environment* current){
    while(current != nullptr && not current->exists(name)){
        current = current->get_parent();
    }
    if(current == nullptr){
        lox::error(value.get_line(), "Undefined variable '" + name + "'.");
        return;
    }
    current->define(name, var(value.get_type(), value.get_literal()));
}

token env::get(token name, environment* current){
    var res = current->get(name.get_lexeme());
    return token(res.get_type(), name.get_lexeme(), res.get_value(), name.get_line());
}

token env::get_arg(std::string name){
    return env::get(token(token_type::IDENTIFIER, name, "", 0));
}


/**
 * @brief 操作环境中函数的函数
*/


bool env::func_exist(std::string name){
    return locale->exists(name) && locale->get(name).get_type() == token_type::FUN;
}

void env::func_define(std::string name, std::vector<token> params, stmt body){
    if(func_exist(name)){
        lox::error(-1, "Function '" + name + "' already declared in this scope.");
        return;
    }
    locale->define      (name, var(token_type::FUN, name));
    locale->define_func (name, params, body);
}

func env::func_search(std::string name){
    environment* current = locale;
    while(current != nullptr &&
         (not current->exists(name) || 
          current->get(name).get_type() != token_type::FUN)){
        current = current->get_parent();
    }
    if(current == nullptr){
        lox::error(-1, "Undefined function '" + name + "'.");
        return func();
    }
    return current->get_func(name);
}


/**
 * @brief 操作环境中类的函数
*/
void env::class_define(std::string name, std::map<std::string, stmt> methods){
    if(locale->exists(name)){
        lox::error(-1, "Class '" + name + "' already declared in this scope.");
        return;
    }
    locale->define(name, var(token_type::CLASS, name));
    stmt_function* constructor = nullptr;
    if(methods.find("init") != methods.end()){
        //constructor = new stmt_function(name, dynamic_cast<stmt_function>( (*methods["init"]) ));
    }
    //locale->define_func(name, 
    for(auto& method : methods){
        method.second->accept();
    }
}