#pragma once

#ifndef __PARSER_HPP__
#define __PARSER_HPP__


#include "token.hpp"
#include "token_type.hpp"
#include "expr.hpp"
#include "stmt.hpp"

#include <vector>
#include <string>


class parser {
private:
    std::vector<token>& tokens;
    int current;

    expr parse_expression();        // 匹配表达式
    expr parse_assignment();        // 匹配赋值
    expr parse_logical_or();        // 匹配逻辑或
    expr parse_logical_and();       // 匹配逻辑与
    expr parse_equality();          // 匹配等式
    expr parse_comparison();        // 匹配比较式
    expr parse_term();              // 匹配项
    expr parse_factor();            // 匹配因子
    expr parse_unary();             // 匹配一元运算符
    expr parse_call();              // 匹配函数调用
    expr parse_primary();           // 匹配基本表达式

    stmt parse_declaration();       // 匹配声明
    stmt parse_var_decl();          // 匹配变量声明
    stmt parse_statement();         // 匹配语句
    stmt parse_expr_stmt();         // 匹配表达式语句
    stmt parse_if_stmt();           // 匹配if语句
    stmt parse_while_stmt();        // 匹配while语句
    stmt parse_for_stmt();          // 匹配for语句
    stmt parse_return_stmt();       // 匹配return语句
    stmt parse_break_stmt();        // 匹配break语句
    stmt parse_block();             // 匹配代码块
    stmt parse_function();          // 匹配函数声明
    stmt parse_class_decl();        // 匹配类声明
    stmt_method* parse_method();    // 匹配类方法

    expr finish_call(expr callee);  // 完成函数调用

    template<typename... T>
    bool match_peer(T... types);

    token advance();

    token get_prev();
    token get_peer();
    token get_next();

    bool is_at_end();

    token consume(token_type, std::string_view);

    void synchronize();
public:
    parser(std::vector<token>&);
    std::vector<stmt> parse();
};



#endif // __PARSER_HPP__