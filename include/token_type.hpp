#pragma once

#ifndef __TOKEN_TYPE_HPP__
#define __TOKEN_TYPE_HPP__

#include <string>
#include <map>

enum class token_type{
    // Single-character tokens
    // 单一字符的token
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    // One or two character tokens
    // 一个或两个字符的token
    BANG,    BANG_EQUAL,
    EQUAL,   EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS,    LESS_EQUAL,

    // Literals
    // 字面量，即数字或字符串
    IDENTIFIER, STRING, NUMBER,

    // Keywords
    // 关键字
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    BREAK, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    // End of file
    // 文件结束
    END
};

inline std::map<std::string, token_type> keywords = {
    {"and",    token_type::AND},
    {"break",  token_type::BREAK},
    {"class",  token_type::CLASS},
    {"else",   token_type::ELSE},
    {"false",  token_type::FALSE},
    {"for",    token_type::FOR},
    {"fun",    token_type::FUN},
    {"if",     token_type::IF},
    {"nil",    token_type::NIL},
    {"or",     token_type::OR},
    {"return", token_type::RETURN},
    {"super",  token_type::SUPER},
    {"this",   token_type::THIS},
    {"true",   token_type::TRUE},
    {"var",    token_type::VAR},
    {"while",  token_type::WHILE}
};



inline std::map<token_type, std::string> token_names = {
    {token_type::IDENTIFIER, "identifier"},
    {token_type::STRING, "string"},
    {token_type::NUMBER, "number"},
    {token_type::AND, "and"},
    {token_type::CLASS, "class"},
    {token_type::ELSE, "else"},
    {token_type::FALSE, "false"},
    {token_type::FUN, "fun"},
    {token_type::FOR, "for"},
    {token_type::IF, "if"},
    {token_type::NIL, "nil"},
    {token_type::OR, "or"},
    {token_type::RETURN, "return"},
    {token_type::SUPER, "super"},
    {token_type::THIS, "this"},
    {token_type::TRUE, "true"},
    {token_type::VAR, "var"},
    {token_type::WHILE, "while"},
    {token_type::BREAK, "break"}
};



#endif // __TOKEN_TYPE_HPP__
