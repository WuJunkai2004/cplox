#pragma once

#ifndef __ENV_HPP__
#define __ENV_HPP__

#include <string>
#include <vector>
#include <map>
#include <set>

#include "token.hpp"
#include "token_type.hpp"
#include "stmt.hpp"


class var{
private:
    token_type type;
    std::string value;
public:
    var();
    var(token_type, std::string);
    token_type  get_type();
    std::string get_value();
};

using symbol_table = std::map<std::string, var>;


class func{
private:
    std::vector<token> params;
    stmt body;
    bool defined = false;
public:
    func();
    func(std::vector<token>, stmt);
    int get_arity();
    stmt get_body();
    std::string get_param(int);
    bool is_defined();
};

using function_table = std::map<std::string, func>;

inline std::set<std::string> class_register;

class environment{
private:
    environment* parent;
    symbol_table table;
    function_table fun;
    std::map<std::string, symbol_table> ins;
public:
    environment(environment* = nullptr);
    ~environment();
    bool exists(std::string);
    void define(std::string, var);
    void define_func(std::string, std::vector<token>, stmt);
    void define_instance(std::string);
    bool exists_instance(std::string);

    var  get(std::string);
    func get_func(std::string);
    environment* get_parent();
};


namespace env{
    inline environment* global;
    inline environment* locale;
    void init();
    void push();
    void pop();

    bool is_global_scope();

    void define(std::string, token, environment* = locale);
    void assign(std::string, token, environment* = locale);

    token get(token, environment* = locale);
    token get_arg(std::string);

    bool func_exist(std::string, environment* current = locale);
    void func_define(std::string, std::vector<token>, stmt, environment* current = locale);
    func func_search(std::string);

    void class_define(std::string, std::map<std::string, stmt_method*>);
}



#endif // __ENV_HPP__