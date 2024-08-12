#include "expr.hpp"

#include "code.hpp"
#include "env.hpp"

#include "token.hpp"
#include "token_type.hpp"

#include "error.hpp"
#include "return.hpp"

#include <iostream>

expr_binary::expr_binary(expr left_, expr right_, token operate_):
    left(left_),
    right(right_),
    operate(operate_)
{}

expr_binary::~expr_binary(){
    delete left;
    delete right;
}

token expr_binary::accept(){
    token l_var = code::interpreter::evaluate(left);
    switch(operate.get_type()){
        case token_type::PLUS:
            return l_var + code::interpreter::evaluate(right);
        case token_type::MINUS:
            return l_var - code::interpreter::evaluate(right);
        case token_type::STAR:
            return l_var * code::interpreter::evaluate(right);
        case token_type::SLASH:
            return l_var / code::interpreter::evaluate(right);
        case token_type::GREATER:
            return l_var > code::interpreter::evaluate(right);
        case token_type::GREATER_EQUAL:
            return l_var >= code::interpreter::evaluate(right);
        case token_type::LESS:
            return l_var < code::interpreter::evaluate(right);
        case token_type::LESS_EQUAL:
            return l_var <= code::interpreter::evaluate(right);
        case token_type::BANG_EQUAL:
            return l_var != code::interpreter::evaluate(right);
        case token_type::EQUAL_EQUAL:
            return l_var == code::interpreter::evaluate(right);
        case token_type::OR:
            if(l_var){
                return l_var;
            }
            return code::interpreter::evaluate(right);
        case token_type::AND:
            if(!l_var){
                return l_var;
            }
            return code::interpreter::evaluate(right);
        default:
            lox::raise_runtime_error(operate.get_line(), "Unknown binary operator.");
    }
    return token(token_type::NIL, "", "", -1);
}

expr_unary::expr_unary(expr right_, token operate_):
    right(right_),
    operate(operate_)
{}

expr_unary::~expr_unary(){
    delete right;
}

token expr_unary::accept(){
    token r_var = code::interpreter::evaluate(right);
    token result(token_type::NIL, "", "", -1);
    switch(operate.get_type()){
        case token_type::MINUS:
            result = token(token_type::NUMBER, "", "0", r_var.get_line()) - r_var;
            break;
        case token_type::BANG:
            result = token(r_var? token_type::FALSE: token_type::TRUE, "", "", r_var.get_line());
            break;
        case token_type::PLUS:
            result = r_var;
            break;
        default:
            lox::raise_runtime_error(operate.get_line(), "Unknown unary operator.");
    }
    return result;
}


expr_literal::expr_literal(token value_):
    value(value_)
{}

expr_literal::~expr_literal(){}

token expr_literal::accept(){
    return value;
}


expr_grouping::expr_grouping(expr expression_):
    expression(expression_)
{}

expr_grouping::~expr_grouping(){
    delete expression;
}

token expr_grouping::accept(){
    return code::interpreter::evaluate(expression);
}


expr_variable::expr_variable(token attr_):
    attr(attr_)
{}

expr_variable::~expr_variable(){}

token expr_variable::accept(){
    token res = env::get(attr);
    return token(res.get_type(), res.get_lexeme(), res.get_literal(), attr.get_line());
}


expr_assign::expr_assign(expr name_, expr value_):
    name(name_),
    value(value_)
{}

expr_assign::~expr_assign(){
    delete name;
    delete value;
}

token expr_assign::accept(){
    token real_name = code::interpreter::evaluate(this->name);
    token res = code::interpreter::evaluate(value);
    env::assign(real_name.get_lexeme(), res);
    return res;
}


expr_call::expr_call(expr callee_, std::vector<expr> arguments_):
    callee(callee_),
    arguments(arguments_)
{}

expr_call::~expr_call(){
    delete callee;
    for(auto arg: arguments){
        delete arg;
    }
}

token expr_call::accept(){
    token callee_var = code::interpreter::evaluate(callee);
    if(callee_var.get_type() != token_type::FUN && callee_var.get_type() != token_type::METHOD){
        lox::raise_runtime_error(callee_var.get_line(), "Can only call functions or classes' methods.");
    }
    func callee_func = env::func_search(callee_var.get_literal());
    if(arguments.size() != callee_func.get_arity()){
        std::cerr << "Expected " << callee_func.get_arity() << " arguments but got " << arguments.size() << "." << std::endl;
        return token(token_type::NIL, "", "", -1);
    }
    std::vector<token> args;
    for(auto arg: arguments){
        args.push_back(code::interpreter::evaluate(arg));
    }
    if(callee_var.get_type() == token_type::FUN){
        return code::interpreter::call(callee_func, args);
    }
    this_stack.into_scope();
    this_stack.set( token( token_type::CLASS, 
                            callee_var.get_lexeme() .substr(0, callee_var.get_lexeme().find('.')), 
                            callee_var.get_literal().substr(0, callee_var.get_literal().find('.')),
                            callee_var.get_line()
                    ));
    token result = code::interpreter::call(callee_func, args);
    this_stack.exit_scope();
    return result;
}


expr_dot::expr_dot(expr object_, token name_):
    object(object_),
    name(name_)
{}

expr_dot::~expr_dot(){
    delete object;
}

token expr_dot::accept(){
    token obj = code::interpreter::evaluate(object);
    if(obj.get_type() != token_type::CLASS){
        lox::raise_runtime_error(obj.get_line(), "Only instances have properties.");
        return token(token_type::NIL, "", "", -1);
    }
    if(class_register.count(obj.get_literal()) == 0){
        lox::raise_runtime_error(obj.get_line(), "Undefined class.");
        return token(token_type::NIL, "", "", -1);
    }
    token method_name(token_type::IDENTIFIER, 
        obj.get_lexeme() + "." + name.get_lexeme(),
        name.get_lexeme(),
        name.get_line()
    );
    return env::get(method_name);
}


expr_this::expr_this(){}

expr_this::~expr_this(){}

token expr_this::accept(){
    return this_stack.view_scope();
}