#pragma once

#ifndef __CODE_HPP__
#define __CODE_HPP__

#include "env.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"

#include <vector>

namespace code{
    void interpret( std::vector<stmt>);

    token evaluate(expr);

    void execute(stmt);
    void execute_block(std::vector<stmt>);

    token call(func, std::vector<token>);
};

#endif // __CODE_HPP__