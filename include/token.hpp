#pragma once

#ifndef __TOKEN_HPP__ 
#define __TOKEN_HPP__

#include "token_type.hpp"


#include <string>
#include <format>

#include <ostream>

class token{
private:
    token_type type;
    std::string lexeme;     // 词素，即词法单元的字符串表示
    std::string literal_str;// 字面量，即词法单元的值
    double      literal_num;
    int line;
public:
    token(token_type, std::string_view, std::string_view, int);

    token_type get_type();
    int        get_line();
    std::string get_lexeme();
    std::string get_literal();

    token operator+(token);
    token operator-(token);
    token operator*(token);
    token operator/(token);

    token operator==(token);
    token operator!=(token);
    token operator< (token);
    token operator> (token);
    token operator<=(token);
    token operator>=(token);

    operator bool();

    friend bool is_same_type(const token*, const token*);
    friend bool is_bool_type(const token*);
    friend bool is_comparable(const token*, const token*);

    friend std::ostream& operator<<(std::ostream&, const token);
};

bool is_same_type(const token*, const token*);
bool is_bool_type(const token*);
bool is_comparable(const token*, const token*);

std::ostream& operator<<(std::ostream&, const token);

#endif // __TOKEN_HPP__
