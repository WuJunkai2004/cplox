#pragma once

#ifndef __EXPR_HPP__
#define __EXPR_HPP__

#include "var.hpp"
#include "token.hpp"
#include "token_type.hpp"

#include <vector>


class expr_base{
public:
    expr_base()  = default;
    virtual ~expr_base() = default;
    virtual var accept() = 0;
    virtual int build()  = 0;
};

using expr = expr_base*;


class expr_binary : public expr_base{
private:
    expr left;
    expr right;
    token operate;
public:
    expr_binary(expr, expr, token);
    ~expr_binary();
    var accept() override final;
    int build()  override final;
};
using expr_logical = expr_binary;


class expr_unary : public expr_base{
private:
    expr right;
    token operate;
public:
    expr_unary(expr, token);
    ~expr_unary();
    var accept() override final;
    int build()  override final;
};


class expr_literal : public expr_base{
private:
    token value;
public:
    expr_literal(token);
    ~expr_literal();
    var accept() override final;
    int build()  override final;
};


class expr_grouping : public expr_base{
private:
    expr expression;
public:
    expr_grouping(expr);
    ~expr_grouping();
    var accept() override final;
    int build()  override final;
};


class expr_variable : public expr_base{
private:
    token attr;
    env::variable pos;
public:
    expr_variable(token);
    ~expr_variable();
    var accept() override final;
    int build()  override final;
};


class expr_assign : public expr_base{
private:
    expr name;
    expr value;
public:
    expr_assign(expr, expr);
    ~expr_assign();
    var accept() override final;
    int build()  override final;
};


class expr_call : public expr_base{
private:
    expr callee;
    std::vector<expr> arguments;
public:
    expr_call(expr, std::vector<expr>);
    ~expr_call();
    var accept() override final;
    int build()  override final;
};


class expr_dot : public expr_base{
private:
    expr object;
    token name;
public:
    expr_dot(expr, token);
    ~expr_dot();
    var accept() override final;
    int build()  override final;
};


class expr_this : public expr_base{
public:
    expr_this();
    ~expr_this();
    var accept() override final;
    int build()  override final;
};

#endif // __EXPR_HPP__