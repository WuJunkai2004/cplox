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