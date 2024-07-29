#pragma once

#ifndef __LOX_HPP__
#define __LOX_HPP__

#include <string>

namespace lox{
    inline bool had_global_error  = false;
    inline bool had_runtime_error = false;
    inline bool had_syntax_error  = false;

    void run(std::string_view);
    void run_file(std::string_view);

    int match_prompt(std::string_view);
    std::string get_prompt();
    void run_prompt();

    void error(int, std::string_view);
    void report(int, std::string_view, std::string_view);
};

#endif // __LOX_HPP__