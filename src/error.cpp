#include "error.hpp"
#include <string>
#include <exception>

// 语法错误
syntax_error::syntax_error(std::string message_) :
    message(message_)
{}

const char* syntax_error::what() const noexcept{
    return message.c_str();
}


// 运行时错误
runtime_error::runtime_error(std::string message_) :
    message(message_)
{}

const char* runtime_error::what() const noexcept{
    return message.c_str();
}