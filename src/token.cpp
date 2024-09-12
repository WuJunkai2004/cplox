#include "token.hpp"
#include "token_type.hpp"
#include "env.hpp"
#include "lox.hpp"

#include <string>
#include <format>

#include <iostream>

/**
 * @brief 开销较小的储存变量的类
*/
var::var():
    type(token_type::NIL),
    value("")
{}

var::var(token_type type_, std::string value_):
    type(type_),
    value(value_)
{}

token_type var::get_type(){
    return type;
}

std::string var::get_value(){
    return value;
}

// 辅助函数
bool is_same_type(const var* a, const var* b){
    return a->type == b->type;
}

bool is_bool_type(const var* a){
    return a->type == token_type::TRUE || a->type == token_type::FALSE;
}

bool is_comparable(const var* a, const var* b){
    return is_same_type(a, b) && (a->type == token_type::NUMBER || a->type == token_type::STRING);
}


// 重载运算符
var var::operator+(var b){
    if(not is_same_type(this, &b)){
        lox::error(-1, "Cannot add different types");
        return var();
    }
    if(this->type == token_type::NUMBER){
        return var(token_type::NUMBER, std::to_string(std::stod(this->value) + std::stod(b.value)));
    } else if(this->type == token_type::STRING){
        return var(token_type::STRING, this->value + b.value);
    }
    lox::error(-1, "Cannot add types");
    return var();
}

var var::operator-(var b){
    if(type != token_type::NUMBER || not is_same_type(this, &b)){
        lox::error(-1, "Cannot subtract non-number types");
        return var();
    }
    return var(token_type::NUMBER, std::to_string(std::stod(this->value) - std::stod(b.value)));
}

var var::operator*(var b){
    if(type != token_type::NUMBER || not is_same_type(this, &b)){
        lox::error(-1, "Cannot multiply non-number types");
        return var();
    }
    return var(token_type::NUMBER, std::to_string(std::stod(this->value) * std::stod(b.value)));
}

var var::operator/(var b){
    if(type != token_type::NUMBER || not is_same_type(this, &b)){
        lox::error(-1, "Cannot divide non-number types");
        return var();
    }
    if(std::stod(b.value) == 0){
        lox::error(-1, "Cannot divide by zero");
        return var();
    }
    return var(token_type::NUMBER, std::to_string(std::stod(this->value) / std::stod(b.value)));
}

var var::operator==(var b){
    if(is_bool_type(this) && is_bool_type(&b)){
        return var((type == b.type) ? token_type::TRUE : token_type::FALSE, "");
    }
    if(not is_same_type(this, &b)){
        lox::error(-1, "Cannot equal with different types");
        return var(token_type::NIL, "");
    }
    if(type == token_type::STRING){
        return var((value == b.value) ? token_type::TRUE : token_type::FALSE, "");
    } 
    if(type == token_type::NUMBER){
        return var((std::stod(value) == std::stod(b.value)) ? token_type::TRUE : token_type::FALSE, "");
    }
    lox::error(-1, "Cannot compare types");
    return var();
}

var var::operator!=(var b){
    return (*this == b) == var(token_type::FALSE, "");
}

var var::operator<(var b){
    if(not is_comparable(this, &b)){
        lox::error(-1, "Cannot compare types");
        return var();
    }
    if(type == token_type::STRING){
        return var((value < b.value) ? token_type::TRUE : token_type::FALSE, "");
    }
    return var((std::stod(value) < std::stod(b.value)) ? token_type::TRUE : token_type::FALSE, "");
}

var var::operator>(var b){
    if(not is_comparable(this, &b)){
        lox::error(-1, "Cannot compare types");
        return var();
    }
    if(type == token_type::STRING){
        return var((value > b.value) ? token_type::TRUE : token_type::FALSE, "");
    }
    return var((std::stod(value) > std::stod(b.value)) ? token_type::TRUE : token_type::FALSE, "");
}

var var::operator<=(var b){
    return (*this > b) == var(token_type::FALSE, "");
}

var var::operator>=(var b){
    return (*this < b) == var(token_type::FALSE, "");
}

var::operator bool(){
    if(type != token_type::NUMBER &&
       type != token_type::STRING &&
       type != token_type::NIL &&
       not is_bool_type(this)){
        lox::error(-1, "Cannot convert to bool");
        return false;
    }
    if(type == token_type::NUMBER){
        return std::stod(value) != 0;
    }
    if(type == token_type::STRING){
        return not value.empty();
    }
    if(type == token_type::FALSE || type == token_type::NIL){
        return false;
    }
    return type == token_type::TRUE;
}

std::ostream& operator<<(std::ostream& os, const var v){
    switch(v.type){
        case token_type::NIL:
            return os << "nil";
        case token_type::NUMBER:
            return os << std::stod(v.value);
        case token_type::TRUE:
            return os << "true";
        case token_type::FALSE:
            return os << "false";
        case token_type::FUN:
            return os << std::format("<fun {0}>", v.value);
        case token_type::CLASS:
            return os << std::format("<class {0}>", v.value);
        case token_type::METHOD:
            return os << std::format("<method {0}>", v.value);
        default:
            return os << v.value;
    }
}


token::token(token_type type_, std::string_view lexeme_, std::string_view literal_, int line_)
    : type(type_), lexeme(lexeme_), literal(literal_), line(line_){}


token_type token::get_type(){
    return type;
}


int token::get_line(){
    return line;
}


std::string token::get_literal(){
    return literal;
}


std::string token::get_lexeme(){
    return lexeme;
}


var token::to_var(){
    return var(type, literal);
}