#pragma once

#ifndef __ENV_HPP__
#define __ENV_HPP__

#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>

#include "token.hpp"
#include "token_type.hpp"
#include "stmt.hpp"


class func{
private:
    std::vector<token> params;
    stmt body;
    int  defined;
public:
    func();
    func(std::vector<token>, stmt);
    func(std::vector<token>, int);
    int get_arity();
    stmt get_body();
    std::string get_param(int);
    int get_defined();
    bool is_defined();
};


using symbol_name = std::vector<std::string>;
using symbol_value = std::vector<var>;
using symbol_table = std::map<std::string, var>;
using function_table = std::map<std::string, func>;

inline std::set<std::string> class_register;
inline symbol_table returned_instance;

namespace env{
    void push();
    void pop();
}


class environment{
private:
    symbol_name    names;
    symbol_value   values;
    function_table fun;
    std::stack<int> scope_stack;
public:
    environment();
    ~environment();
    int  exists(std::string, bool=false);
    int  define(std::string, var);
    int  assign(int,         var);
    void define_func(std::string, std::vector<token>, int);
    void define_func(std::string, std::vector<token>, stmt);

    var  get(std::string);
    var  get(int);
    func get_func(std::string);

    symbol_table get_this();

    friend void env::push();
    friend void env::pop();
};


namespace env{
    inline environment* global;
    inline environment* locale;
    inline int scope_depth = 0;

    void init();

    environment* get_current();

    void define(std::string, var);
    void assign(std::string, var);
    void assign(variable, var);

    var get(std::string);
    var get(variable);
    var get(token);
    variable get_arg(std::string);

    bool func_exist(std::string);
    void func_define(std::string, int, int);
    void func_define(std::string, std::vector<token>, stmt);
    func func_search(std::string);

    void class_define(std::string, std::map<std::string, stmt_method*>);
}

/** new env struct **/

#endif // __ENV_HPP__