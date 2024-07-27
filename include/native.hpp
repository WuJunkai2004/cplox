#pragma once

#ifndef __NATIVE_HPP__
#define __NATIVE_HPP__

#include "stmt.hpp"
#include <chrono>

// 原生函数
namespace native{
    using native_func = stmt_base;

    class func_clock : public native_func{
    private:
        using time_type = std::chrono::time_point<std::chrono::system_clock>;
        using dura_type = std::chrono::duration<double>;
        time_type start;
    public:
        func_clock();
        void accept() override;
    };


    class func_print : public native_func{
    public:
        void accept() override;
    };


    class func_input : public native_func{
    public:
        void accept() override;
    };


    class func_num : public native_func{
    public:
        void accept() override;
    };


    class func_str : public native_func{
    public:
        void accept() override;
    };


    class func_exit : public native_func{
    public:
        void accept() override;
    };


    class func_int : public native_func{
    public:
        void accept() override;
    };


    class func_sleep : public native_func{
    public:
        void accept() override;
    };


    void import();
}


#endif // __NATIVE_HPP__