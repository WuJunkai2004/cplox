#include "stmt.hpp"
#include "token.hpp"
#include "token_type.hpp"
#include "expr.hpp"
#include "token.hpp"
#include "env.hpp"
#include "code.hpp"
#include "return.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <map>


stmt_expr::stmt_expr(expr expression_):
    expression(expression_)
{}

stmt_expr::~stmt_expr(){
    delete expression;
}

void stmt_expr::accept(){
    code::interpreter::evaluate(expression);
}

int stmt_expr::build(){
    return expression->build();
}



stmt_var::stmt_var(std::string name_, expr value_):
    name(name_),
    value(value_)
{}

stmt_var::~stmt_var(){
    delete value;
}

void stmt_var::accept(){
    env::define(name, code::interpreter::evaluate(value));
}

int stmt_var::build(){
    int value_line = value->build();
    code::compiler::assemble(operation_code::CONSTANT, name);
    code::compiler::assemble(operation_code::SET_ITEM);
    return value_line + 2;
}


stmt_block::stmt_block(std::vector<stmt> statements_):
    statements(statements_)
{}

stmt_block::~stmt_block(){
    for(auto& statement : statements){
        delete statement;
    }
}

void stmt_block::accept(){
    code::interpreter::execute_block(statements);
}

int stmt_block::build(){
    int total_line = 2;
    code::compiler::assemble(operation_code::SET_LOCAL);
    for(auto& statement : statements){
        total_line += statement->build();
    }
    code::compiler::assemble(operation_code::END_LOCAL);
    return total_line;
}


stmt_if::stmt_if(expr condition_, stmt then_branch_, stmt else_branch_):
    condition(condition_),
    then_branch(then_branch_),
    else_branch(else_branch_)
{}

stmt_if::~stmt_if(){
    delete condition;
    delete then_branch;
    delete else_branch;
}

void stmt_if::accept(){
    if(code::interpreter::evaluate(condition)){
        code::interpreter::execute(then_branch);
    }else if(else_branch != nullptr){
        code::interpreter::execute(else_branch);
    }
}

int stmt_if::build(){
    condition->build();
    int then_index = code::compiler::assemble(operation_code::JUMP_IF_F);
    int then_line = then_branch->build();
    int else_index = 0;
    int else_line = 0;
    int jump_for_more = else_branch != nullptr;
    if(else_branch != nullptr){
        else_index = code::compiler::assemble(operation_code::JUMP);
        else_line = else_branch->build();
        code::compiler::modify(then_index, operation_code::JUMP, std::to_string(else_line));
    }
    code::compiler::modify(then_index, operation_code::JUMP_IF_F, std::to_string(then_line + jump_for_more));
    return 1 + then_line + jump_for_more + else_line;
}


stmt_loop::stmt_loop(expr condition_, stmt body_):
    init(nullptr),
    cond(condition_),
    body(body_),
    incr(nullptr)
{}

stmt_loop::stmt_loop(stmt init_, expr condotion_, expr increment_, stmt body_):
    init(init_),
    cond(condotion_),
    body(body_),
    incr(increment_)
{}

stmt_loop::~stmt_loop(){
    delete init;
    delete cond;
    delete incr;
    delete body;
}

void stmt_loop::accept(){
    env::push();
    if(init != nullptr){
        code::interpreter::execute(init);
    }
    brk_stack.into_scope();
    while(code::interpreter::evaluate(cond)){
        code::interpreter::execute(body);
        if(brk_stack.has_set()){
            break;
        }
        if(incr != nullptr){
            code::interpreter::evaluate(incr);
        }
    }
    brk_stack.exit_scope();
    env::pop();
}

int stmt_loop::build(){
    code::compiler::assemble(operation_code::SET_LOCAL);
    int init_line = 0;
    if(init != nullptr){
        init_line = init->build();
    }
    int body_line = body->build();
    int incr_line = 0;
    if(incr != nullptr){
        incr_line = incr->build();
    }
    int cond_line = cond->build();
    code::compiler::assemble(operation_code::NEGATE);
    code::compiler::assemble(operation_code::JUMP_IF_F, std::to_string(body_line + incr_line + cond_line + 2));
    code::compiler::assemble(operation_code::END_LOCAL);
    return 1 + init_line + body_line + incr_line + cond_line + 3;
}


stmt_function::stmt_function(std::string name_, std::vector<token> params_, stmt body_):
    name(name_),
    params(params_),
    body(body_)
{}

stmt_function::~stmt_function(){
    delete body;
}

void stmt_function::accept(){
    env::func_define(name, params, body);
}

int stmt_function::build(){
    code::compiler::assemble(operation_code::FUNC, name+","+std::to_string(params.size()));
    int ignore_pos = code::compiler::assemble(operation_code::JUMP);
    code::compiler::assemble(operation_code::SET_LOCAL);
    for(auto& param : params){
        code::compiler::assemble(operation_code::CONSTANT, param.get_lexeme());
        code::compiler::assemble(operation_code::SET_ITEM);
    }
    int body_line = body->build();
    code::compiler::assemble(operation_code::END_LOCAL);
    code::compiler::modify(ignore_pos, operation_code::JUMP, std::to_string(2 + 2 * params.size() + body_line));
    return 3 + 2 * params.size() + body_line + 1;
}


stmt_return::stmt_return(expr value_):
    value(value_)
{}

stmt_return::~stmt_return(){
    delete value;
}

void stmt_return::accept(){
    ret_stack.set(code::interpreter::evaluate(value));
}

int stmt_return::build(){
    int value_line = value->build();
    code::compiler::assemble(operation_code::RETURN);
    return value_line + 1;
}


stmt_break::stmt_break(){}

void stmt_break::accept(){
    brk_stack.set();
}

int stmt_break::build(){
    // todo
    //code::compiler::assemble(operation_code::BREAK);
    return 0;
}


stmt_class::stmt_class(std::string name_, std::map<std::string, stmt_method*> methods_):
    name(name_),
    methods(methods_)
{}

stmt_class::~stmt_class(){
    for(auto& method : methods){
        delete method.second;
    }
}

void stmt_class::accept(){
    env::class_define(name, methods);
}

int stmt_class::build(){
    // todo
    return 0;
}


stmt_init::stmt_init(std::string name_, stmt constructor_):
    class_name(name_),
    constructor(constructor_)
{}

stmt_init::~stmt_init(){
    delete constructor;
}

void stmt_init::accept(){
    token this_token(token_type::CLASS, "this", class_name, -1);        // 创建this变量
    env::define("this", this_token);                                    // 将this变量加入环境
    this_stack.into_scope();                                            // 深入this栈
    this_stack.set( this_token );                                       // 将this变量加入this栈
    code::interpreter::execute( constructor );                                       // 执行初始化函数
    ret_stack.set( this_token );                                        // 返回this变量
    this_stack.exit_scope();                                            // 退出this栈
    returned_instance = env::locale->get_this();                        // 获取返回的实例
}

int stmt_init::build(){
    // todo
    return 0;
}