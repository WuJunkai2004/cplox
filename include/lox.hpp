#pragma once

#ifndef __LOX_HPP__
#define __LOX_HPP__

#include <string>
#include <filesystem>

namespace lox{
    inline bool had_global_error  = false;
    inline bool had_runtime_error = false;
    inline bool had_syntax_error  = false;

    enum class file_mode{
        UNCOMPILED,
        COMPILED,
    };

    void repl_mode();                       // REPL mode, read-eval-print-loop
    void ast_mode(std::string_view);        // AST mode, parse to AST and accept
    void compile_mode(std::string_view);    // Compile mode, compile to bytecode
    void interpret_mode(std::string_view);  // Interpret mode, interpret bytecode

    file_mode get_file_mode(std::filesystem::path);

    void run(std::string_view);

    int match_prompt(std::string_view);
    std::string get_prompt();

    void error(int, std::string_view);
    void report(int, std::string_view, std::string_view);
};

#endif // __LOX_HPP__