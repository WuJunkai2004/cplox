#include "code.hpp"
#include "env.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"
#include "return.hpp"

#include <iostream>


void code::interpret( std::vector<stmt> statements){
    try{
        for(auto& statement : statements){
            execute(statement);
            if(ret_stack.has_set() || brk_stack.has_set()){
                return;
            }
        }
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}


void code::execute(stmt statement){
    if(statement == nullptr){
        return;
    }
    statement->accept();
}


token code::evaluate(expr expression){
    if(expression == nullptr){
        return token(token_type::NIL, "", "", -1);
    }
    return expression->accept();
}


void code::execute_block(std::vector<stmt> statements){
    env::push();
    interpret(statements);
    env::pop();
}


token code::call(func function, std::vector<token> arguments){
    env::push();
    ret_stack.into_scope();
    for(int i = 0; i < function.get_arity(); i++){
        env::define(function.get_param(i), arguments[i]);
    }
    execute(function.get_body());
    env::pop();
    return ret_stack.exit_scope();
}