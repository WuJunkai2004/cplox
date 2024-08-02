#pragma once

#ifndef __RETURN_HPP__
#define __RETURN_HPP__

#include <vector>
#include "token.hpp"

class return_stack{
private:
    int current;
    struct return_data{
        token value;
        bool  is_set;
    };
    std::vector<return_data> rets;
public:
    return_stack();

    void  into_scope();
    token view_scope();
    token exit_scope();
    void  set();
    void  set(token);

    bool  has_set();
};

inline return_stack ret_stack;
inline return_stack brk_stack;
inline return_stack this_stack;

#endif // __RETURN_HPP__