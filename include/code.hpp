#pragma once

#ifndef __CODE_HPP__
#define __CODE_HPP__

#include "env.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"
#include "vm.hpp"

#include <vector>

namespace code{
    std::vector<token> generate_tokens(std::string_view);
    std::vector<stmt>  generate_ast(std::vector<token>);

    namespace interpreter{
        void interpret(std::vector<stmt>);

        token evaluate(expr);

        void execute(stmt);
        void execute_block(std::vector<stmt>);

        token call(func, std::vector<token>);
    }

    namespace compiler{
        void compile(std::vector<stmt>);
    }

    namespace vm{
        void interpret(std::vector<stmt>);
    }
};

#endif // __CODE_HPP__