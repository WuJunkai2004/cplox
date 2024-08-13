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
        void accept() override final;
        int  build() override final;
    };


    class func_print : public native_func{
    public:
        void accept() override final;
        int  build() override final;
    };


    class func_input : public native_func{
    public:
        void accept() override final;
        int  build() override final;
    };


    class func_num : public native_func{
    public:
        void accept() override final;
        int  build() override final;
    };


    class func_str : public native_func{
    public:
        void accept() override final;
        int  build() override final;
    };


    class func_exit : public native_func{
    public:
        void accept() override final;
        int  build() override final;
    };


    class func_int : public native_func{
    public:
        void accept() override final;
        int  build() override final;
    };


    class func_sleep : public native_func{
    public:
        void accept() override final;
        int  build() override final;
    };


    class file_init : public native_func{
    public:
        void accept() override final;
        int  build() override final;
    };

    class file_read : public native_func{
    public:
        void accept() override final;
        int  build() override final;
    };

    class file_write : public native_func{
    public:
        void accept() override final;
        int  build() override final;
    };


    void import();
    void regist();
}


#endif // __NATIVE_HPP__