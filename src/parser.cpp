#include "parser.hpp"
#include "expr.hpp"
#include "error.hpp"
#include "token.hpp"
#include "token_type.hpp"
#include "lox.hpp"

#include <string>
#include <vector>
#include <map>
#include <iostream>


/**
 * @brief 递归下降解析器的辅助函数
*/

token parser::advance(){
    if(!is_at_end()){
        current++;
    }
    return get_prev();
}

bool parser::is_at_end(){
    return get_peer().get_type() == token_type::END;
}

token parser::get_prev(){
    return tokens[current - 1];
}

token parser::get_peer(){
    return tokens[current];
}

token parser::get_next(){
    return tokens[current + 1];
}

template<typename... T>
bool parser::match_peer(T... types){
    static_assert((std::is_same_v<T, token_type> && ...), 
                  "All arguments must be of type token_type");
    for(auto type : {types...}){
        if(get_peer().get_type() == type){
            advance();
            return true;
        }
    }
    return false;
}


/* rule
expression     → equality ;
equality       → comparison ( ( "!=" | "==" ) comparison )* ;
comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term           → factor ( ( "-" | "+" ) factor )* ;
factor         → unary ( ( "/" | "*" ) unary )* ;
unary          → ( "!" | "-" ) unary
               | primary ;
primary        → NUMBER | STRING | "true" | "false" | "nil"
               | "(" expression ")" ;
*/

parser::parser(std::vector<token>& tokens_):
    tokens(tokens_),
    current(0)
{}


std::vector<stmt> parser::parse(){
    std::vector<stmt> statements;
    while(not is_at_end()){
        statements.push_back(parse_declaration());
    }
    return statements;
}


/**
 * @brief 表达式解析
*/
expr parser::parse_expression(){
    return parse_assignment();
}


expr parser::parse_assignment(){
    expr the_expr = parse_logical_or();
    if(get_peer().get_type() == token_type::EQUAL){
        token name = get_prev();
        advance();
        expr value = parse_assignment();
        return new expr_assign(name.get_lexeme(), value);
    }
    return the_expr;
}


expr parser::parse_dot(){
    expr the_expr = parse_term();

    while(match_peer(token_type::DOT)){
        token name = consume(token_type::IDENTIFIER, "Expect property name after '.'.");
        the_expr = new expr_get(the_expr, name);
    }

    return the_expr;
}


expr parser::parse_logical_or(){
    expr the_expr = parse_logical_and();

    while(match_peer(token_type::OR)){
        token op = get_prev();
        expr right = parse_equality();
        the_expr = new expr_logical(the_expr, right, op);
    }

    return the_expr;
}


expr parser::parse_logical_and(){
    expr the_expr = parse_equality();

    while(match_peer(token_type::AND)){
        token op = get_prev();
        expr right = parse_equality();
        the_expr = new expr_logical(the_expr, right, op);
    }

    return the_expr;
}


expr parser::parse_equality(){
    expr the_expr = parse_comparison();

    while(match_peer(token_type::BANG_EQUAL, token_type::EQUAL_EQUAL)){
        token op = get_prev();
        expr right = parse_comparison();
        the_expr = new expr_binary(the_expr, right, op);
    }

    return the_expr;
}

expr parser::parse_comparison(){
    expr the_expr = parse_term();

    while(match_peer(token_type::GREATER, token_type::GREATER_EQUAL, 
                     token_type::LESS,    token_type::LESS_EQUAL )){
        token op = get_prev();
        expr right = parse_term();
        the_expr = new expr_binary(the_expr, right, op);
    }

    return the_expr;
}


expr parser::parse_term(){
    expr the_expr = parse_factor();

    while(match_peer(token_type::MINUS, token_type::PLUS)){
        token op = get_prev();
        expr right = parse_factor();
        the_expr = new expr_binary(the_expr, right, op);
    }

    return the_expr;
}

expr parser::parse_factor(){
    expr the_expr = parse_unary();

    while(match_peer(token_type::SLASH, token_type::STAR)){
        token op = get_prev();
        expr right = parse_unary();
        the_expr = new expr_binary(the_expr, right, op);
    }

    return the_expr;
}


expr parser::parse_unary(){
    if(match_peer(token_type::BANG, token_type::MINUS)){
        token op = get_prev();
        expr right = parse_unary();
        return new expr_unary(right, op);
    }

    return parse_call();
}


expr parser::parse_call(){
    expr the_expr = parse_primary();

    while(true){
        if(match_peer(token_type::LEFT_PAREN)){
            the_expr = finish_call(the_expr);
        } else if (match_peer(token_type::DOT)){
            token name = consume(token_type::IDENTIFIER, "Expect property name after '.'.");
            the_expr = new expr_get(the_expr, name);
        } else {
            break;
        }
    }

    return the_expr;
}


expr parser::finish_call(expr callee){
    std::vector<expr> arguments;
    if(get_peer().get_type() != token_type::RIGHT_PAREN){
        do{
            if(arguments.size() >= 255){
                lox::error(get_peer().get_line(), "Can't have more than 255 arguments.");
            }
            arguments.push_back(parse_expression());
        } while(match_peer(token_type::COMMA));
    }

    consume(token_type::RIGHT_PAREN, "Expect ')' after arguments.");
    return new expr_call(callee, arguments);
}


expr parser::parse_primary(){
    if(match_peer(token_type::NUMBER, token_type::STRING, 
                  token_type::TRUE,   token_type::FALSE, 
                  token_type::NIL)){
        return new expr_literal(get_prev());
    }

    if(match_peer(token_type::IDENTIFIER)){
        if(get_peer().get_type() == token_type::DOT){
            token name = get_prev();
            advance();
            token attr = consume(token_type::IDENTIFIER, "Expect property name after '.'.");
            return new expr_get(new expr_variable(name), attr);
        }
        return new expr_variable(get_prev());
    }

    if(match_peer(token_type::LEFT_PAREN)){
        expr expr = parse_expression();
        consume(token_type::RIGHT_PAREN, "Expect ')' after expression.");
        return new expr_grouping(expr);
    }

    return nullptr;
}


/**
 * @brief 语句解析
*/
stmt parser::parse_declaration(){
    try{
        if(match_peer(token_type::CLASS)){
            return parse_class_decl();
        }
        if(match_peer(token_type::FUN)){
            return parse_function();
        }
        if(match_peer(token_type::VAR)){
            return parse_var_decl();
        }
        return parse_statement();
    }catch(std::exception){
        synchronize();
        return nullptr;
    }
}


stmt parser::parse_var_decl(){
    token name = consume(token_type::IDENTIFIER, "Expect variable name.");
    expr initializer = nullptr;

    if(match_peer(token_type::EQUAL)){
        initializer = parse_expression();
    } else if(get_peer().get_type() == token_type::SEMICOLON){
        initializer = new expr_literal(token(token_type::NIL, "", "", get_peer().get_line()));
    }
    consume(token_type::SEMICOLON, "Expect ';' after variable declaration.");
    return new stmt_var(name.get_lexeme(), initializer);
}


stmt parser::parse_class_decl(){
    token name = consume(token_type::IDENTIFIER, "Expect class name.");
    consume(token_type::LEFT_BRACE, "Expect '{' before class body.");

    std::map<std::string, stmt_method*> methods;
    while(not is_at_end() and match_peer(token_type::FUN)){
        std::string method_name = get_peer().get_lexeme();
        methods[method_name] = parse_method();
    }
    consume(token_type::RIGHT_BRACE, "Expect '}' after class body.");
    return new stmt_class(name.get_lexeme(), methods);
}


stmt_method* parser::parse_method(){
    token name = consume(token_type::IDENTIFIER, "Expect method name.");
    consume(token_type::LEFT_PAREN, "Expect '(' after method name.");

    std::vector<token> parameters;
    if(get_peer().get_type() != token_type::RIGHT_PAREN){
        do{
            if(parameters.size() >= 255){
                lox::error(get_peer().get_line(), "Can't have more than 255 parameters.");
            }
            parameters.push_back(consume(token_type::IDENTIFIER, "Expect parameter name."));
        } while(match_peer(token_type::COMMA));
    }
    consume(token_type::RIGHT_PAREN, "Expect ')' after parameters.");

    consume(token_type::LEFT_BRACE, "Expect '{' before method body.");
    stmt body = parse_block();
    return new stmt_method(name.get_lexeme(), parameters, body);
}


stmt parser::parse_statement(){
    if(match_peer(token_type::FOR)){
        return parse_for_stmt();
    }
    if(match_peer(token_type::IF)){
        return parse_if_stmt();
    }
    if(match_peer(token_type::RETURN)){
        return parse_return_stmt();
    }
    if(match_peer(token_type::BREAK)){
        return parse_break_stmt();
    }
    if(match_peer(token_type::WHILE)){
        return parse_while_stmt();
    }
    if(match_peer(token_type::LEFT_BRACE)){
        return parse_block();
    }
    return parse_expr_stmt();
}


stmt parser::parse_expr_stmt(){
    expr value = parse_expression();
    consume(token_type::SEMICOLON, "Expect ';' after value.");
    return new stmt_expr(value);
}


stmt parser::parse_if_stmt(){
    consume(token_type::LEFT_PAREN, "Expect '(' after 'if'.");
    expr condition = parse_expression();
    consume(token_type::RIGHT_PAREN, "Expect ')' after if condition.");

    stmt then_branch = parse_statement();
    stmt else_branch = nullptr;
    if(match_peer(token_type::ELSE)){
        else_branch = parse_statement();
    }

    return new stmt_if(condition, then_branch, else_branch);
}


stmt parser::parse_while_stmt(){
    consume(token_type::LEFT_PAREN, "Expect '(' after 'while'.");
    expr condition = parse_expression();
    consume(token_type::RIGHT_PAREN, "Expect ')' after while condition.");

    stmt body = parse_statement();
    return new stmt_while(condition, body);
}


stmt parser::parse_for_stmt(){
    consume(token_type::LEFT_PAREN, "Expect '(' after 'for'.");
    stmt initializer = nullptr;
    if(match_peer(token_type::SEMICOLON)){
        initializer = nullptr;
    } else if(match_peer(token_type::VAR)){
        initializer = parse_var_decl();
    } else {
        initializer = parse_expr_stmt();
    }

    expr condition = nullptr;
    if(get_peer().get_type() != token_type::SEMICOLON){
        condition = parse_expression();
    }
    consume(token_type::SEMICOLON, "Expect ';' after loop condition.");

    expr increment = nullptr;
    if(get_peer().get_type() != token_type::RIGHT_PAREN){
        increment = parse_expression();
    }
    consume(token_type::RIGHT_PAREN, "Expect ')' after for clauses.");

    stmt body = parse_statement();

    if(condition == nullptr){
        condition = new expr_literal(token(token_type::TRUE, "", "", get_peer().get_line()));
    }
    return new stmt_for(initializer, condition, increment, body);
}


stmt parser::parse_return_stmt(){
    token keyword = get_prev();
    expr value = nullptr;
    if(get_peer().get_type() != token_type::SEMICOLON){
        value = parse_expression();
    }
    consume(token_type::SEMICOLON, "Expect ';' after return value.");
    return new stmt_return(value);
}


stmt parser::parse_break_stmt(){
    consume(token_type::SEMICOLON, "Expect ';' after break.");
    return new stmt_break();
}


stmt parser::parse_block(){
    std::vector<stmt> statements;
    while(not is_at_end() and get_peer().get_type() != token_type::RIGHT_BRACE){
        statements.push_back(parse_declaration());
    }
    consume(token_type::RIGHT_BRACE, "Expect '}' after block.");
    return new stmt_block(statements);
}


stmt parser::parse_function(){
    // return parse_method();
    token name = consume(token_type::IDENTIFIER, "Expect function name.");
    consume(token_type::LEFT_PAREN, "Expect '(' after function name.");
    std::vector<token> parameters;
    if(get_peer().get_type() != token_type::RIGHT_PAREN){
        do{
            if(parameters.size() >= 255){
                lox::error(get_peer().get_line(), "Can't have more than 255 parameters.");
            }
            parameters.push_back(consume(token_type::IDENTIFIER, "Expect parameter name."));
        } while(match_peer(token_type::COMMA));
    }
    consume(token_type::RIGHT_PAREN, "Expect ')' after parameters.");

    consume(token_type::LEFT_BRACE, "Expect '{' before function body.");
    stmt body = parse_block();
    return new stmt_function(name.get_lexeme(), parameters, body);
}


/**
 * @brief 报错处理
*/
token parser::consume(token_type type, std::string_view message){
    if(get_peer().get_type() == type){
        return advance();
    }

    //throw error(get_peer(), message);
    lox::error(get_peer().get_line(), message);
    return token(token_type::NIL, "", "", get_peer().get_line());
}


void parser::synchronize(){
    advance();

    while(not is_at_end()){
        if(get_prev().get_type() == token_type::SEMICOLON){
            return;
        }

        switch(get_peer().get_type()){
            case token_type::CLASS:
            case token_type::FUN:
            case token_type::VAR:
            case token_type::FOR:
            case token_type::IF:
            case token_type::WHILE:
            case token_type::RETURN:
                return;
            default:
                break;
        }

        advance();
    }
}