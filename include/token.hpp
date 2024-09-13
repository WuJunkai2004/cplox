#pragma once

#ifndef __TOKEN_HPP__ 
#define __TOKEN_HPP__

#include "token_type.hpp"

#include <string>
#include <ostream>

namespace env{
    enum scope_type{
        GLOBAL,
        LOCAL,
        UNKNOWN
    };

    struct variable{
        scope_type scope;
        int        offset;

        variable();
        variable(scope_type, int);
    };
}


class var{
private:
    token_type type;
    std::string value;
public:
    env::variable pos;
    var();
    var(token_type, std::string);
    token_type  get_type();
    std::string get_value();

    var operator+(var);
    var operator-(var);
    var operator*(var);
    var operator/(var);

    var operator==(var);
    var operator!=(var);
    var operator< (var);
    var operator> (var);
    var operator<=(var);
    var operator>=(var);

    operator bool();

    friend bool is_same_type(const var*, const var*);
    friend bool is_bool_type(const var*);
    friend bool is_comparable(const var*, const var*);

    friend std::ostream& operator<<(std::ostream&, const var);
};

bool is_same_type(const var*, const var*);
bool is_bool_type(const var*);
bool is_comparable(const var*, const var*);


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

    var to_var();
};

#endif // __TOKEN_HPP__
