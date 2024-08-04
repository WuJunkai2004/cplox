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


bool env::is_global_scope(){
    return locale == global;
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
    std::size_t dot_pos = name.find('.');
    std::string member = "";
    if(dot_pos != std::string::npos){
        member = name.substr(dot_pos + 1);
        name = name.substr(0, dot_pos);
    }
    while(current != nullptr && not current->exists(name)){
        current = current->get_parent();
    }
    if(current == nullptr){
        lox::error(value.get_line(), "Undefined variable '" + name + "'.");
        return;
    }
    if(not member.empty()){
        name = name + "." + member;
    }
    current->define(name, var(value.get_type(), value.get_literal()));
}

token env::get(token name, environment* current){
    // find the dot
    std::size_t dot_pos = name.get_lexeme().find('.');
    if(dot_pos == std::string::npos){
        var res = current->get(name.get_lexeme());
        return token(res.get_type(), name.get_lexeme(), res.get_value(), name.get_line());
    }
    std::string ins_name = name.get_lexeme().substr(0, dot_pos);
    // 查询实例是否存在
    while(current != nullptr && not current->exists(ins_name)){
        current = current->get_parent();
    }
    if(current == nullptr){
        lox::error(name.get_line(), "Undefined instance '" + ins_name + "'.");
        return token(token_type::NIL, name.get_lexeme(), name.get_lexeme(), name.get_line());
    }
    var ins = current->get(ins_name);
    std::string method_name = name.get_lexeme().substr(dot_pos + 1);
    if(func_exist(ins.get_value()+"."+method_name, current)){
        return token(token_type::METHOD, ins_name+"."+method_name, ins.get_value()+"."+method_name, name.get_line());
    }
    if(not current->exists(name.get_lexeme())){
        return token(token_type::NIL, name.get_lexeme(), name.get_lexeme(), name.get_line());
    }
    var res = current->get(name.get_lexeme());
    return token(res.get_type(), name.get_lexeme(), res.get_value(), name.get_line());
}

token env::get_arg(std::string name){
    return env::get(token(token_type::IDENTIFIER, name, "", 0));
}


/**
 * @brief 操作环境中函数的函数
*/


bool env::func_exist(std::string name, environment* current){
    return current->exists(name) && current->get(name).get_type() == token_type::FUN;
}

void env::func_define(std::string name, std::vector<token> params, stmt body, environment* current){
    if(func_exist(name, current)){
        lox::error(-1, "Function '" + name + "' already declared in this scope.");
        return;
    }
    current->define      (name, var(token_type::FUN, name));
    current->define_func (name, params, body);
}

func env::func_search(std::string name){
    // 如果有dot
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
void env::class_define(std::string name, std::map<std::string, stmt_method*> methods){
    if(not is_global_scope()){
        lox::error(-1, "Classes can only be defined in global scope.");
        return;
    }
    if(global->exists(name)){
        lox::error(-1, "The name '" + name + "' already declared.");
        return;
    }
    if(methods.find("init") == methods.end()){          // 如果没有初始化函数
        lox::error(-1, "Class '" + name + "' must have an initializer.");
        return;
    }
    class_register.insert(name);                        // 将类名加入类注册表
    define(name, token(token_type::CLASS, name, "", 0), global); // 在全局环境中定义类名
    func_define(name, 
        methods["init"]->params, 
        new stmt_init(name, methods["init"]),
    global); // 在全局环境中定义初始化函数
    for(auto& method : methods){
        if(method.first == "init"){
            continue;
        }
        func_define(name + "." + method.first,
            method.second->params, 
            method.second->body,
            global
        ); // 在全局环境中定义类的方法
    }
}