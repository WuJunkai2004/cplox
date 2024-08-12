#pragma once

#ifndef __STMT_HPP__
#define __STMT_HPP__

#include "expr.hpp"
#include "token.hpp"
#include <string>
#include <vector>

class stmt_base{
public:
    stmt_base() = default;
    virtual ~stmt_base() = default;
    virtual void accept() = 0;
};


using stmt = stmt_base*;


class stmt_expr : public stmt_base{
private:
    expr expression;
public:
    stmt_expr(expr);
    ~stmt_expr();
    void accept() override final;
};


class stmt_var : public stmt_base{
private:
    std::string name;
    expr value;
public:
    stmt_var(std::string, expr);
    ~stmt_var();
    void accept() override final;
};


class stmt_block : public stmt_base{
private:
    std::vector<stmt> statements;
public:
    stmt_block(std::vector<stmt>);
    ~stmt_block();
    void accept() override final;
};


class stmt_if : public stmt_base{
private:
    expr condition;
    stmt then_branch;
    stmt else_branch;
public:
    stmt_if(expr, stmt, stmt);
    ~stmt_if();
    void accept() override final;
};


class stmt_loop : public stmt_base{
private:
    stmt init;
    expr cond;
    expr incr;
    stmt body;
public:
    stmt_loop(expr, stmt);
    stmt_loop(stmt, expr, expr, stmt);
    ~stmt_loop();
    void accept() override final;
};
using stmt_while = stmt_loop;
using stmt_for   = stmt_loop;


class stmt_function : public stmt_base{
public:
    std::string name;
    std::vector<token> params;
    stmt body;
    stmt_function(std::string, std::vector<token>, stmt);
    ~stmt_function();
    void accept() override final;
};
using stmt_method = stmt_function;


class stmt_return : public stmt_base{
private:
    expr value;
public:
    stmt_return(expr);
    ~stmt_return();
    void accept() override final;
};


class stmt_break : public stmt_base{
public:
    stmt_break();
    void accept() override final;
};


class stmt_class : public stmt_base{
private:
    std::string name;
    std::map<std::string, stmt_method*> methods;
public:
    stmt_class(std::string, std::map<std::string, stmt_method*>);
    ~stmt_class();
    void accept() override final;
};


class stmt_init : public stmt_base{
private:
    std::string class_name;
    stmt constructor;
public:
    stmt_init(std::string, stmt);
    ~stmt_init();
    void accept() override final;
};


#endif // __STMT_HPP__