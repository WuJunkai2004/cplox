#include "error.hpp"
#include <string>
#include <format>
#include <exception>

#include "lox.hpp"

// 语法错误
lox::syntax_error::syntax_error(std::string message_) :
    message(message_)
{}

const char* lox::syntax_error::what() const noexcept{
    return message.c_str();
}


// 运行时错误
lox::runtime_error::runtime_error(std::string message_) :
    message(message_)
{}

const char* lox::runtime_error::what() const noexcept{
    return message.c_str();
}


void lox::raise_syntax_error(int line, std::string message){
    lox::had_syntax_error = true;
    lox::error(line, message);
    throw syntax_error(message);
}


void lox::raise_runtime_error(int line, std::string message){
    lox::had_runtime_error = true;
    lox::error(line, message);
    throw runtime_error(message);
}