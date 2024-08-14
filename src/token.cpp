#include "token.hpp"
#include "token_type.hpp"
#include "env.hpp"
#include "lox.hpp"

#include <string>
#include <format>

#include <iostream>


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
        return token(token_type::NUMBER, "", std::to_string(std::stod(this->literal) + std::stod(b.literal)), this->line);
    } else if(this->type == token_type::STRING){
        return token(token_type::STRING, "", this->literal + b.literal, this->line);
    }
    lox::error(this->line, "Cannot add types");
    return token(token_type::NIL, "", "", this->line);
}


token token::operator-(token b){
    if(type != token_type::NUMBER || not is_same_type(this, &b)){
        lox::error(this->line, "Cannot subtract non-number types");
        return token(token_type::NIL, "", "", this->line);
    }
    return token(token_type::NUMBER, "", std::to_string(std::stod(this->literal) - std::stod(b.literal)), this->line);
}


token token::operator*(token b){
    if(type != token_type::NUMBER || not is_same_type(this, &b)){
        lox::error(this->line, "Cannot multiply non-number types");
        return token(token_type::NIL, "", "", this->line);
    }
    return token(token_type::NUMBER, "", std::to_string(std::stod(this->literal) * std::stod(b.literal)), this->line);
}


token token::operator/(token b){
    if(type != token_type::NUMBER || not is_same_type(this, &b)){
        lox::error(this->line, "Cannot divide non-number types");
        return token(token_type::NIL, "", "", this->line);
    }
    if(std::stod(b.literal) == 0){
        lox::error(this->line, "Cannot divide by zero");
        return token(token_type::NIL, "", "", this->line);
    }
    return token(token_type::NUMBER, "", std::to_string(std::stod(this->literal) / std::stod(b.literal)), this->line);
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
        return token((literal == b.literal) ? token_type::TRUE : token_type::FALSE, "", "", this->line);
    } 
    if(type == token_type::NUMBER){
        return token((std::stod(literal) == std::stod(b.literal)) ? token_type::TRUE : token_type::FALSE, "", "", this->line);
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
        return token((literal < b.literal) ? token_type::TRUE : token_type::FALSE, "", "", this->line);
    }
    return token((std::stod(literal) < std::stod(b.literal)) ? token_type::TRUE : token_type::FALSE, "", "", this->line);
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
        return token((literal > b.literal) ? token_type::TRUE : token_type::FALSE, "", "", this->line);
    }
    return token((std::stod(literal) > std::stod(b.literal)) ? token_type::TRUE : token_type::FALSE, "", "", this->line);
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
        return std::stod(literal) != 0;
    }
    if(type == token_type::STRING){
        return not literal.empty();
    }
    if(type == token_type::FALSE || type == token_type::NIL){
        return false;
    }
    return type == token_type::TRUE;
}


std::string token::to_string(){
    switch(type){
        case token_type::TRUE:
            return "true";
        case token_type::FALSE:
            return "false";
        case token_type::NIL:
            return "nil";
        case token_type::STRING:{
            std::string res="\"";
            for(auto c : literal){
                switch(c){
                    case '\n':
                        res += "\\n";
                        break;
                    case '\r':
                        res += "\\r";
                        break;
                    case '\t':
                        res += "\\t";
                        break;
                    case '\\':
                        res += "\\\\";
                        break;
                    case '"':
                        res += "\\\"";
                        break;
                    default:
                        res += c;
                        break;
                }
            }
            res += "\"";
            return res;
        }
        case token_type::NUMBER:
            return literal;
        default:
            return "nil";
    }
}


std::ostream& operator<<(std::ostream& os, const token t){
    switch(t.type){
        case token_type::NIL:
            os << "nil";
            break;
        case token_type::NUMBER:
            os << std::stod(t.literal);
            break;
        case token_type::TRUE:
            os << "true";
            break;
        case token_type::FALSE:
            os << "false";
            break;
        case token_type::FUN:
            os << std::format("<fun {0}>", t.literal);
            break;
        case token_type::CLASS:
            os << std::format("<class {0}>", t.literal);
            break;
        case token_type::METHOD:
            os << std::format("<method {0}>", t.literal);
            break;
        default:
            os << t.literal;
            break;
    }
    return os;
}
