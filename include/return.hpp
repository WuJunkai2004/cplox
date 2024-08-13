#pragma once

#ifndef __RETURN_HPP__
#define __RETURN_HPP__

#include <vector>
#include "token.hpp"

class vm_stack{
private:
    int current;
    struct return_data{
        token value;
        bool  is_set;
    };
    std::vector<return_data> rets;
public:
    vm_stack();

    void  into_scope();
    token view_scope();
    token exit_scope();
    void  set();
    void  set(token);

    bool  has_set();

    token top();
    token pop();
    void  push(token);
};

// 解释AST需要的栈
inline vm_stack ret_stack;
inline vm_stack brk_stack;
inline vm_stack this_stack;

// 解释bytecode需要的栈
inline vm_stack value_stack;


#endif // __RETURN_HPP__