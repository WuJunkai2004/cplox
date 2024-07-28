#pragma once

#ifndef __ERROR_HPP__
#define __ERROR_HPP__

#include <string>
#include <exception>

namespace lox{
    // 语法错误
    class syntax_error : public std::exception{
    private:
        std::string message;
    public:
        syntax_error(std::string);
        const char* what() const noexcept override;
    };


    // 运行时错误
    class runtime_error : public std::exception{
    private:
        std::string message;
    public:
        runtime_error(std::string);
        const char* what() const noexcept override;
    };


    void raise_syntax_error(int, std::string);
    void raise_runtime_error(int, std::string);
};


#endif // __ERROR_HPP__