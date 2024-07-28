#pragma once

#ifndef __EXPR_HPP__
#define __EXPR_HPP__

#include "token.hpp"
#include "token_type.hpp"

#include <vector>


class expr_base{
public:
    expr_base()  = default;
    virtual ~expr_base() = default;
    virtual token accept() = 0;
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
    token accept() override;
};
using expr_logical = expr_binary;


class expr_unary : public expr_base{
private:
    expr right;
    token operate;
public:
    expr_unary(expr, token);
    ~expr_unary();
    token accept() override;
};


class expr_literal : public expr_base{
private:
    token value;
public:
    expr_literal(token);
    ~expr_literal();
    token accept() override;
};


class expr_grouping : public expr_base{
private:
    expr expression;
public:
    expr_grouping(expr);
    ~expr_grouping();
    token accept() override;
};


class expr_variable : public expr_base{
private:
    token attr;
public:
    expr_variable(token);
    ~expr_variable();
    token accept() override;
};


class expr_assign : public expr_base{
private:
    std::string name;
    expr value;
public:
    expr_assign(std::string, expr);
    ~expr_assign();
    token accept() override;
};


class expr_call : public expr_base{
private:
    expr callee;
    std::vector<expr> arguments;
public:
    expr_call(expr, std::vector<expr>);
    ~expr_call();
    token accept() override;
};


class expr_get : public expr_base{
private:
    expr object;
    token name;
public:
    expr_get(expr, token);
    ~expr_get();
    token accept() override;
};

#endif // __EXPR_HPP__