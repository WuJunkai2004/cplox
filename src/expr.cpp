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

var expr_binary::accept(){
    var l_var = code::interpreter::evaluate(left);
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
    return var();
}

int expr_binary::build(){
    int r_line_num = right->build();
    int l_line_num = left->build();
    code::compiler::assemble(operate.get_type());
    return l_line_num + r_line_num + 1;
}


expr_unary::expr_unary(expr right_, token operate_):
    right(right_),
    operate(operate_)
{}

expr_unary::~expr_unary(){
    delete right;
}

var expr_unary::accept(){
    var r_var = code::interpreter::evaluate(right);
    var result = var();
    switch(operate.get_type()){
        case token_type::MINUS:
            result = var(token_type::NUMBER, "0") - r_var;
            break;
        case token_type::BANG:
            result = var(r_var? token_type::FALSE: token_type::TRUE, "");
            break;
        case token_type::PLUS:
            result = r_var;
            break;
        default:
            lox::raise_runtime_error(operate.get_line(), "Unknown unary operator.");
    }
    return result;
}

int expr_unary::build(){
    int line_num = right->build();
    code::compiler::assemble(operate.get_type());
    return line_num + 1;
}


expr_literal::expr_literal(token value_):
    value(value_)
{}

expr_literal::~expr_literal(){}

var expr_literal::accept(){
    return value.to_var();
}

int expr_literal::build(){
    code::compiler::assemble(value.get_type(), value.get_literal());
    return 1;
}


expr_grouping::expr_grouping(expr expression_):
    expression(expression_)
{}

expr_grouping::~expr_grouping(){
    delete expression;
}

var expr_grouping::accept(){
    return code::interpreter::evaluate(expression);
}

int expr_grouping::build(){
    return expression->build();
}


expr_variable::expr_variable(token attr_):
    attr(attr_)
{}

expr_variable::~expr_variable(){}

var expr_variable::accept(){
    if(offset.scope == env::scope_type::UNKNOWN){
        offset = env::get_arg(attr.get_lexeme());
    }
    var ret = env::get(offset);
    ret.pos = offset;
    return ret;
}

int expr_variable::build(){
    code::compiler::assemble(operation_code::GET_ITEM, attr.get_lexeme());
    return 1;
}


expr_assign::expr_assign(expr name_, expr value_):
    name(name_),
    value(value_)
{}

expr_assign::~expr_assign(){
    delete name;
    delete value;
}

var expr_assign::accept(){
    var real_name = code::interpreter::evaluate(this->name);
    var res = code::interpreter::evaluate(value);
    env::assign(real_name.pos, res);
    return res;
}

int expr_assign::build(){
    int value_line_num = value->build();
    int name_line_num  = name->build();
    code::compiler::assemble(operation_code::SET_ITEM);
    return value_line_num + name_line_num + 1;
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

var expr_call::accept(){
    var callee_var = code::interpreter::evaluate(callee);
    if(callee_var.get_type() != token_type::FUN && callee_var.get_type() != token_type::METHOD){
        lox::raise_runtime_error(-1, "Can only call functions or classes' methods.");
    }
    func callee_func = env::func_search(callee_var.get_value());
    if(arguments.size() != callee_func.get_arity()){
        std::cerr << "Expected " << callee_func.get_arity() << " arguments but got " << arguments.size() << "." << std::endl;
        return var();
    }
    std::vector<var> args;
    for(auto arg: arguments){
        args.push_back(code::interpreter::evaluate(arg));
    }
    if(callee_var.get_type() == token_type::FUN){
        return code::interpreter::call(callee_func, args);
    }
    this_stack.into_scope();
    this_stack.set( var( token_type::CLASS,
                         callee_var.get_value().substr(0, callee_var.get_value().find('.'))
    ));
    var result = code::interpreter::call(callee_func, args);
    this_stack.exit_scope();
    return result;
}

int expr_call::build(){
    int total_line_num = 0;
    for(int idx=arguments.size()-1; idx>=0; idx--){
        total_line_num += arguments[idx]->build();
    }
    total_line_num += callee->build();
    code::compiler::assemble(operation_code::CALL, std::to_string(arguments.size()));
    return total_line_num + 1;
}


expr_dot::expr_dot(expr object_, token name_):
    object(object_),
    name(name_)
{}

expr_dot::~expr_dot(){
    delete object;
}

var expr_dot::accept(){
    var obj = code::interpreter::evaluate(object);
    if(obj.get_type() != token_type::CLASS){
        lox::raise_runtime_error(-1, "Only instances have properties.");
        return var();
    }
    if(class_register.count(obj.get_value()) == 0){
        lox::raise_runtime_error(-1, "Undefined class.");
        return var();
    }
    token method_name(token_type::IDENTIFIER, 
        obj.get_value() + "." + name.get_lexeme(),
        name.get_lexeme(),
        name.get_line()
    );
    return env::get(method_name);
}

int expr_dot::build(){
    int obj_line_num = object->build();
    //code::compiler::assemble(operation_code::GET_PROPERTY, name.to_string());
    return obj_line_num + 1;
}


expr_this::expr_this(){}

expr_this::~expr_this(){}

var expr_this::accept(){
    return this_stack.view_scope();
}

int expr_this::build(){
    code::compiler::assemble(operation_code::GET_ITEM, "this");
    return 1;
}