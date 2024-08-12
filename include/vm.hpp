#pragma once

#ifndef __vm_hpp__
#define __vm_hpp__

enum class operation_code{
    CONSTANT,        // 常量操作
    NIL,             // 空值操作
    TRUE,            // 布尔真值操作
    FALSE,           // 布尔假值操作
    POP,             // 弹出栈顶操作
    GET_LOCAL,       // 获取局部变量操作
    SET_LOCAL,       // 设置局部变量操作
    GET_GLOBAL,      // 获取全局变量操作
    DEFINE_GLOBAL,   // 定义全局变量操作
    SET_GLOBAL,      // 设置全局变量操作
    GET_UPVALUE,     // 获取上值操作
    SET_UPVALUE,     // 设置上值操作
    GET_PROPERTY,    // 获取属性操作
    SET_PROPERTY,    // 设置属性操作
    GET_SUPER,       // 获取超类操作
    EQUAL,           // 等于操作
    GREATER,         // 大于操作
    LESS,            // 小于操作
    ADD,             // 加法操作
    SUBTRACT,        // 减法操作
    MULTIPLY,        // 乘法操作
    DIVIDE,          // 除法操作
    NOT,             // 非操作
    NEGATE,          // 取反操作
    JUMP,            // 跳转操作
    JUMP_IF_FALSE,   // 如果为假则跳转操作
    LOOP,            // 循环操作
    CALL,            // 调用操作
    INVOKE,          // 调用方法操作
    SUPER_INVOKE,    // 调用超类方法操作
    CLOSURE,         // 闭包操作
    CLOSE_UPVALUE,   // 关闭上值操作
    RETURN,          // 返回操作
    CLASS,           // 类操作
    INHERIT,         // 继承操作
    METHOD,          // 方法操作
    ILLEGAL          // 非法操作
};


operation_code get_operation(const int code);


#endif // __vm_hpp__