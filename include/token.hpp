#pragma once

#ifndef __TOKEN_HPP__ 
#define __TOKEN_HPP__

#include "token_type.hpp"


#include <string>

#include <ostream>

class token{
private:
    token_type type;
    std::string lexeme;     // 词素，即词法单元的字符串表示
    std::string literal;    // 字面量，即词法单元的值
    int line;
public:
    token() = default;
    token(token_type, std::string_view, std::string_view, int);

    token_type get_type();
    int        get_line();
    std::string get_lexeme();
    std::string get_literal();

    std::string to_string();
};

#endif // __TOKEN_HPP__
