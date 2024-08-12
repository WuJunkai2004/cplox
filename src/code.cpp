#include "code.hpp"
#include "env.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"
#include "return.hpp"
#include "parser.hpp"
#include "scanner.hpp"

#include <iostream>


std::vector<token> code::generate_tokens(std::string_view source){
    scanner token_scanner(source);
    return token_scanner.scan_tokens();
}


std::vector<stmt> code::generate_ast(std::vector<token> tokens){
    parser token_parser(tokens);
    return token_parser.parse();
}


void code::interpreter::interpret(std::vector<stmt> statements){
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


void code::interpreter::execute(stmt statement){
    if(statement == nullptr){
        return;
    }
    statement->accept();
}


token code::interpreter::evaluate(expr expression){
    if(expression == nullptr){
        return token(token_type::NIL, "", "", -1);
    }
    return expression->accept();
}


void code::interpreter::execute_block(std::vector<stmt> statements){
    env::push();
    interpret(statements);
    env::pop();
}


token code::interpreter::call(func function, std::vector<token> arguments){
    env::push();
    ret_stack.into_scope();
    for(int i = 0; i < function.get_arity(); i++){
        env::define(function.get_param(i), arguments[i]);
    }
    execute(function.get_body());
    env::pop();
    return ret_stack.exit_scope();
}