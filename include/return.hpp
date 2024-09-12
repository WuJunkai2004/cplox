#pragma once

#ifndef __RETURN_HPP__
#define __RETURN_HPP__

#include <vector>
#include "token.hpp"

class vm_stack{
private:
    int current;
    struct return_data{
        var  value;
        bool is_set;
    };
    std::vector<return_data> rets;
public:
    vm_stack();

    void into_scope();
    var  view_scope();
    var  exit_scope();
    void set();
    void set(var);

    bool  has_set();

    var  top();
    var  pop();
    void push(var);
};

// 解释AST需要的栈
inline vm_stack ret_stack;
inline vm_stack brk_stack;
inline vm_stack this_stack;

// 解释bytecode需要的栈
inline vm_stack value_stack;


#endif // __RETURN_HPP__