#include "env.hpp"
#include "lox.hpp"
#include "token.hpp"
#include "token_type.hpp"
#include "stmt.hpp"

#include <string>
#include <map>
#include <algorithm>

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



/**
 * @brief 变量的环境类
*/
environment::environment():
    names(),
    values(),
    fun()
{
    scope_stack.push(0);
}

environment::~environment(){}

int environment::exists(std::string name){
    auto pos = std::find(names.begin() + scope_stack.top(), names.end(), name);
    if(pos != names.end()){
        return pos - names.begin();
    }
    return -1;
}

int environment::define(std::string name, var value){
    names .push_back(name);
    values.push_back(value);
    return values.size() - 1;
}

int environment::assign(int id, var value){
    values[id] = value;
    if(value.get_type() == token_type::CLASS && not returned_instance.empty() && names[id] != "this"){
        for(auto& vars : returned_instance){
            define(names[id] + "." + vars.first, vars.second);
        }
        returned_instance.clear();
    }
    return id;
}

void environment::define_func(std::string name, std::vector<token> params, int defined){
    fun[name] = func(params, defined);
}

void environment::define_func(std::string name, std::vector<token> params, stmt body){
    fun[name] = func(params, body);
}

var environment::get(std::string name){
    int id = exists(name);
    if(id != -1){
        return values[id];
    }
    lox::error(-1, "Undefined variable '" + name + "'.");
    return var();
}

var environment::get(int id){
    return values[id];
}

func environment::get_func(std::string name){
    return fun[name];
}

symbol_table environment::get_this(){
    symbol_table res;
    for(auto var_name : names){
        if(var_name.length() > 5 && var_name.substr(0, 5) == "this."){
            res[var_name.substr(5)] = values[exists(var_name)];
        }
    }
    return res;
}


/**
 * @brief 操作环境的函数
*/
void env::init(){
    global = new environment();
    locale = new environment();
}

void env::push(){
    scope_depth++;
    locale->scope_stack.push(locale->names.size());
}

void env::pop(){
    if(scope_depth){
        scope_depth--;
    }
    locale->scope_stack.pop();
}

environment* env::get_current(){
    if(scope_depth){
        return locale;
    }
    return global;
}


/**
 * @brief 操作环境中变量的函数
*/
void env::define(std::string name, var value){
    environment* current = get_current();
    if(current->exists(name) != -1){
        lox::error(-1, "Variable '" + name + "' already declared in this scope.");
        return;
    }
    current->define(name, value);
}

void env::assign(std::string name, var value){
    int locale_id = locale->exists(name);
    if(locale_id != -1){
        locale->assign(locale_id, value);
    }
    int global_id = global->exists(name);
    if(global_id != -1){
        global->assign(global_id, value);
    }
    lox::error(-1, "Undefined variable '" + name + "'.");
}

var env::get(token name){
    /** @todo */
    // find the dot
    std::size_t dot_pos = name.get_lexeme().find('.');
    if(dot_pos == std::string::npos){
        return get(name.get_lexeme()) ;
    }
    std::string ins_name = name.get_lexeme().substr(0, dot_pos);
    // 查询实例是否存在
    var ins = get(ins_name);
    std::string method_name = name.get_lexeme().substr(dot_pos + 1);
    if(func_exist(ins.get_value()+"."+method_name)){
        return var(token_type::METHOD, ins.get_value()+"."+method_name);
    }
    return get(name.get_lexeme());
}

var env::get(std::string name){
    return get(get_arg(name));
}

var env::get(env::variable pos){
    switch(pos.scope){
        case GLOBAL:
            return global->get(pos.offset);
        case LOCAL:
            return locale->get(pos.offset);
        default:
            return var();
    }
}

env::variable env::get_arg(std::string name){
    int locale_id = locale->exists(name);
    if(locale_id != -1){
        return variable{LOCAL, locale_id};
    }
    int global_id = global->exists(name);
    if(global_id != -1){
        return variable{GLOBAL, global_id};
    }
    lox::error(-1, "Undefined variable '" + name + "'.");
    return variable{};
}


/**
 * @brief 操作环境中函数的函数
*/


bool env::func_exist(std::string name){
    int locale_id = locale->exists(name);
    if(locale_id != -1){
        return locale->get_func(name).is_defined();
    }
    int global_id = global->exists(name);
    if(global_id != -1){
        return global->get_func(name).is_defined();
    }
    return false;
}

void env::func_define(std::string name, int arity_num, int defined_pos){
    if(func_exist(name)){
        lox::error(-1, "Function '" + name + "' already declared in this scope.");
        return;
    }
    environment* current = get_current();
    current->define      (name, var(token_type::FUN, name));
    current->define_func (name, std::vector<token>(arity_num), defined_pos);
}

void env::func_define(std::string name, std::vector<token> params, stmt body){
    if(func_exist(name)){
        lox::error(-1, "Function '" + name + "' already declared in this scope.");
        return;
    }
    environment* current = get_current();
    current->define      (name, var(token_type::FUN, name));
    current->define_func (name, params, body);
}

func env::func_search(std::string name){
    // 如果有dot
    if(locale->exists(name) != -1){
        return locale->get_func(name);
    }
    if(global->exists(name) != -1){
        return global->get_func(name);
    }
    lox::error(-1, "Undefined function '" + name + "'.");
    return func();
}


/**
 * @brief 操作环境中类的函数
*/
void env::class_define(std::string name, std::map<std::string, stmt_method*> methods){
    if(scope_depth){
        lox::error(-1, "Class can only be defined in global scope.");
        return;
    }
    if(global->exists(name) != -1){
        lox::error(-1, "The name '" + name + "' already declared.");
        return;
    }
    if(methods.find("init") == methods.end()){          // 如果没有初始化函数
        lox::error(-1, "Class '" + name + "' must have an initializer.");
        return;
    }
    class_register.insert(name);                        // 将类名加入类注册表
    func_define(name, 
        methods["init"]->params, 
        new stmt_init(name, methods["init"]->body)
    ); // 在全局环境中定义初始化函数
    for(auto& method : methods){
        if(method.first == "init"){
            continue;
        }
        func_define(name + "." + method.first,
            method.second->params, 
            method.second->body
        ); // 在全局环境中定义类的方法
    }
}