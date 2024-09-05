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
#include "var.hpp"


using symbol_name = std::vector<std::string>;
using symbol_value = std::vector<var>;
using symbol_table = std::map<std::string, var>;
using function_table = std::map<std::string, func>;

inline std::set<std::string> class_register;
inline symbol_table returned_instance;

class environment{
private:
    symbol_name    names;
    symbol_value   values;
    function_table fun;
    std::stack<int> scope_stack;
public:
    environment();
    ~environment();
    int  exists(std::string);
    int  define(std::string, var);
    int  assign(int,         var);
    void define_func(std::string, std::vector<token>, int);
    void define_func(std::string, std::vector<token>, stmt);

    var  get(std::string);
    var  get(int);
    func get_func(std::string);

    symbol_table get_this();
};


namespace env{
    inline environment* global;
    inline environment* locale;
    int scope_depth = 0;

    void init();
    void push();
    void pop();
    environment* get_current();

    void define(std::string, var);
    void assign(std::string, var);

    enum scope_type{
        GLOBAL,
        LOCAL,
        UNKNOWN
    };

    struct variable{
        scope_type scope;
        int        offset;
    };

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