#pragma once

#ifndef __CODE_HPP__
#define __CODE_HPP__

#include "env.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"
#include "return.hpp"
#include "vm.hpp"

#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

namespace code{
    std::vector<token> generate_tokens(std::string_view);
    std::vector<stmt>  generate_ast(std::vector<token>);
    std::vector<bcode> generate_bcode(std::vector<stmt>);
    std::vector<bcode> generate_bcode(std::filesystem::path);

    namespace interpreter{
        void interpret(std::vector<stmt>);

        token evaluate(expr);

        void execute(stmt);
        void execute_block(std::vector<stmt>);

        token call(func, std::vector<token>);
    }


    namespace compiler{
        inline std::vector<bcode> byte_code_array;

        void compile(std::vector<stmt>);

        bcode generate_node(token_type, std::string="");
        bcode generate_node(operation_code, std::string="");

        // 创建字节码，返回字节码的索引
        int assemble(token_type, std::string="");
        int assemble(operation_code, std::string="");

        // 修改字节码
        void modify(int, token_type, std::string="");
        void modify(int, operation_code, std::string="");

        void save(std::filesystem::path);
    }


    namespace vm{
        void interpret(std::vector<bcode>);

        int call(vm_stack&, int);
    }
};

#endif // __CODE_HPP__