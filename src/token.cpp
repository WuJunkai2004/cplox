#include "token.hpp"
#include "token_type.hpp"
#include "env.hpp"
#include "lox.hpp"

#include <string>
#include <format>

#include <iostream>


token::token(token_type type_, std::string_view lexeme_, std::string_view literal_, int line_): 
    type(type_), 
    lexeme(lexeme_), 
    line(line_){
    if(type == token_type::NUMBER){
        literal_num = std::stod(std::string(literal_));
    } else {
        literal_str = std::string(literal_);
    }
}


token_type token::get_type(){
    return type;
}


int token::get_line(){
    return line;
}


std::string token::get_literal(){
    switch(type){
        case token_type::NUMBER:
            return std::to_string(literal_num);
        case token_type::FUN:
            [[fallthrough]];
        case token_type::STRING:
            return literal_str;
        default:
            return token_names[type];
    }
}


std::string token::get_lexeme(){
    return lexeme;
}


// 辅助函数
bool is_same_type(const token* a, const token* b){
    return a->type == b->type;
}

bool is_bool_type(const token* a){
    return a->type == token_type::TRUE || a->type == token_type::FALSE;
}

bool is_comparable(const token* a, const token* b){
    return is_same_type(a, b) && (a->type == token_type::NUMBER || a->type == token_type::STRING);
}


// 重载运算符
token token::operator+(token b){
    if(not is_same_type(this, &b)){
        lox::error(this->line, "Cannot add different types");
        return token(token_type::NIL, "", "", this->line);
    }
    if(this->type == token_type::NUMBER){
        return token(token_type::NUMBER, "", std::to_string(this->literal_num + b.literal_num), this->line);
    } else if(this->type == token_type::STRING){
        return token(token_type::STRING, "", this->literal_str + b.literal_str, this->line);
    }
    lox::error(this->line, "Cannot add types");
    return token(token_type::NIL, "", "", this->line);
}


token token::operator-(token b){
    if(type != token_type::NUMBER || not is_same_type(this, &b)){
        lox::error(this->line, "Cannot subtract non-number types");
        return token(token_type::NIL, "", "", this->line);
    }
    return token(token_type::NUMBER, "", std::to_string(this->literal_num - b.literal_num), this->line);
}


token token::operator*(token b){
    if(type != token_type::NUMBER || not is_same_type(this, &b)){
        lox::error(this->line, "Cannot multiply non-number types");
        return token(token_type::NIL, "", "", this->line);
    }
    return token(token_type::NUMBER, "", std::to_string(this->literal_num * b.literal_num), this->line);
}


token token::operator/(token b){
    if(type != token_type::NUMBER || not is_same_type(this, &b)){
        lox::error(this->line, "Cannot divide non-number types");
        return token(token_type::NIL, "", "", this->line);
    }
    if(b.literal_num == 0){
        lox::error(b.line, "Cannot divide by zero");
        return token(token_type::NIL, "", "", b.line);
    }
    return token(token_type::NUMBER, "", std::to_string(this->literal_num / b.literal_num), this->line);
}


token token::operator==(token b){
    if(is_bool_type(this) && is_bool_type(&b)){
        return token((type == b.type) ? token_type::TRUE : token_type::FALSE, "", "", this->line);
    }
    if(not is_same_type(this, &b)){
        lox::error(this->line, "Cannot compare different types");
        return token(token_type::NIL, "", "", this->line);
    }
    if(type == token_type::STRING){
        return token((literal_str == b.literal_str) ? token_type::TRUE : token_type::FALSE, "", "", this->line);
    } 
    if(type == token_type::NUMBER){
        return token((literal_num == b.literal_num) ? token_type::TRUE : token_type::FALSE, "", "", this->line);
    }
    lox::error(this->line, "Cannot compare types");
    return token(token_type::NIL, "", "", this->line);
}


token token::operator!=(token b){
    return (*this == b) == token(token_type::FALSE, "", "", this->line);
}


token token::operator<(token b){
    if(not is_comparable(this, &b)){
        lox::error(this->line, "Cannot compare types");
        return token(token_type::NIL, "", "", this->line);
    }
    if(type == token_type::STRING){
        return token((literal_str < b.literal_str) ? token_type::TRUE : token_type::FALSE, "", "", this->line);
    }
    return token((literal_num < b.literal_num) ? token_type::TRUE : token_type::FALSE, "", "", this->line);
}


token token::operator>=(token b){
    return *this < b == token(token_type::FALSE, "", "", this->line);
}


token token::operator>(token b){
    if(not is_comparable(this, &b)){
        lox::error(this->line, "Cannot compare types");
        return token(token_type::NIL, "", "", this->line);
    }
    if(type == token_type::STRING){
        return token((literal_str > b.literal_str) ? token_type::TRUE : token_type::FALSE, "", "", this->line);
    }
    return token((literal_num > b.literal_num) ? token_type::TRUE : token_type::FALSE, "", "", this->line);
}


token token::operator<=(token b){
    return *this > b == token(token_type::FALSE, "", "", this->line);
}


token::operator bool(){
    if(type != token_type::NUMBER &&
       type != token_type::STRING &&
       type != token_type::NIL &&
       not is_bool_type(this)){
        lox::error(this->line, "Cannot convert to bool");
        lox::had_global_error = true;
        return false;
    }
    if(type == token_type::NUMBER){
        return literal_num != 0;
    }
    if(type == token_type::STRING){
        return not literal_str.empty();
    }
    if(type == token_type::FALSE || type == token_type::NIL){
        return false;
    }
    return type == token_type::TRUE;
}


std::ostream& operator<<(std::ostream& os, const token t){
    switch(t.type){
        case token_type::NIL:
            os << "nil";
            break;
        case token_type::NUMBER:
            os << t.literal_num;
            break;
        case token_type::TRUE:
            os << "true";
            break;
        case token_type::FALSE:
            os << "false";
            break;
        case token_type::FUN:
            os << std::format("<fun {0}>", t.literal_str);
            break;
        default:
            os << t.literal_str;
            break;
    }
    return os;
}
