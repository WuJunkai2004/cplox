#pragma once

#ifndef __vm_hpp__
#define __vm_hpp__

#include "token.hpp"
#include "env.hpp"
#include <stack>
#include <string>
#include <vector>
#include <map>

enum class operation_code{
    CONSTANT,        // 常量操作        = 从常量表中取出常量，压入栈顶
    NIL,             // 空值操作        = 压入空值到栈顶
    TRUE,            // 布尔真值操作    = 压入布尔真值到栈顶
    FALSE,           // 布尔假值操作    = 压入布尔假值到栈顶
    POP,             // 弹出栈顶操作    = 弹出栈顶元素
    GET_ITEM,        // 获取元素操作    = 获取常量表，压入栈顶
    SET_ITEM,        // 设置元素操作    = 将栈顶元素加入常量表
    SET_VARIABLE,    // 设置变量操作    = 设置变量操作
    SET_LOCAL,       // 设置局部变量操作 = 进入新的作用域
    END_LOCAL,       // 结束局部变量操作 = 退出当前作用域
    GET_UPVALUE,     // 获取上值操作
    SET_UPVALUE,     // 设置上值操作
    GET_PROPERTY,    // 获取属性操作
    SET_PROPERTY,    // 设置属性操作
    GET_SUPER,       // 获取超类操作
    NOT_EQUAL,       // 不等操作        = 不等操作，压入栈顶
    EQUAL,           // 等于操作        = 等于操作，压入栈顶
    GREATER,         // 大于操作        = 大于操作，压入栈顶
    GREATER_EQUAL,   // 大于等于操作    = 大于等于操作，压入栈顶
    LESS,            // 小于操作        = 小于操作，压入栈顶
    LESS_EQUAL,      // 小于等于操作    = 小于等于操作，压入栈顶
    ADD,             // 加法操作        = 将栈顶两个元素相加，压入栈顶
    SUBTRACT,        // 减法操作        = 将栈顶两个元素相减，压入栈顶
    MULTIPLY,        // 乘法操作        = 将栈顶两个元素相乘，压入栈顶
    DIVIDE,          // 除法操作        = 将栈顶两个元素相除，压入栈顶
    AND,             // 与操作          = 与操作，压入栈顶
    OR,              // 或操作          = 或操作，压入栈顶
    NEGATE,          // 取反操作        = 取反操作，压入栈顶
    JUMP,            // 跳转操作        = 跳转指定行，可正可负
    JUMP_TO,         // 跳转到操作      = 跳转到指定行
    JUMP_IF_F,         // 条件跳转操作    = 条件跳转到指定行(条件为假时跳转，条件为真时继续执行)
    LOOP,            // 循环操作
    CALL,            // 调用操作        = 调用函数操作
    FUNC,            // 函数操作        = 创建函数操作
    INVOKE,          // 调用方法操作    = 调用方法操作
    SUPER_INVOKE,    // 调用超类方法操作
    CLOSURE,         // 闭包操作
    CLOSE_UPVALUE,   // 关闭上值操作
    RETURN,          // 返回操作        = 返回操作
    BREAK,           // 中断操作        = 中断操作
    CLASS,           // 类操作          = 创建类操作
    INHERIT,         // 继承操作        = 继承操作
    METHOD,          // 方法操作        = 创建方法操作
    ILLEGAL          // 非法操作
};

inline std::map<operation_code, std::string> opcode_map = {
    {operation_code::CONSTANT,      "CONSTANT"},
    {operation_code::NIL,           "NIL"},
    {operation_code::TRUE,          "TRUE"},
    {operation_code::FALSE,         "FALSE"},
    {operation_code::POP,           "POP"},
    {operation_code::GET_ITEM,      "GET_ITEM"},
    {operation_code::SET_ITEM,      "SET_ITEM"},
    {operation_code::SET_VARIABLE,  "SET_VARIABLE"},
    {operation_code::SET_LOCAL,     "SET_LOCAL"},
    {operation_code::END_LOCAL,     "END_LOCAL"},
    {operation_code::GET_UPVALUE,   "GET_UPVALUE"},
    {operation_code::SET_UPVALUE,   "SET_UPVALUE"},
    {operation_code::GET_PROPERTY,  "GET_PROPERTY"},
    {operation_code::SET_PROPERTY,  "SET_PROPERTY"},
    {operation_code::GET_SUPER,     "GET_SUPER"},
    {operation_code::NOT_EQUAL,     "NOT_EQUAL"},
    {operation_code::EQUAL,         "EQUAL"},
    {operation_code::GREATER,       "GREATER"},
    {operation_code::GREATER_EQUAL, "GREATER_EQUAL"},
    {operation_code::LESS,          "LESS"},
    {operation_code::LESS_EQUAL,    "LESS_EQUAL"},
    {operation_code::ADD,           "ADD"},
    {operation_code::SUBTRACT,      "SUBTRACT"},
    {operation_code::MULTIPLY,      "MULTIPLY"},
    {operation_code::DIVIDE,        "DIVIDE"},
    {operation_code::AND,           "AND"},
    {operation_code::OR,            "OR"},
    {operation_code::NEGATE,        "NEGATE"},
    {operation_code::JUMP,          "JUMP"},
    {operation_code::JUMP_TO,       "JUMP_TO"},
    {operation_code::JUMP_IF_F,     "JUMP_IF_F"},
    {operation_code::LOOP,          "LOOP"},
    {operation_code::CALL,          "CALL"},
    {operation_code::FUNC,          "FUNC"},
    {operation_code::INVOKE,        "INVOKE"},
    {operation_code::SUPER_INVOKE,  "SUPER_INVOKE"},
    {operation_code::CLOSURE,       "CLOSURE"},
    {operation_code::CLOSE_UPVALUE, "CLOSE_UPVALUE"},
    {operation_code::RETURN,        "RETURN"},
    {operation_code::BREAK,         "BREAK"},
    {operation_code::CLASS,         "CLASS"},
    {operation_code::INHERIT,       "INHERIT"},
    {operation_code::METHOD,        "METHOD"},
    {operation_code::ILLEGAL,       "ILLEGAL"}
};

inline std::map<std::string, operation_code> opcode_map_reverse = {
    {"CONSTANT",      operation_code::CONSTANT},
    {"NIL",           operation_code::NIL},
    {"TRUE",          operation_code::TRUE},
    {"FALSE",         operation_code::FALSE},
    {"POP",           operation_code::POP},
    {"GET_ITEM",      operation_code::GET_ITEM},
    {"SET_ITEM",      operation_code::SET_ITEM},
    {"SET_VARIABLE",  operation_code::SET_VARIABLE},
    {"SET_LOCAL",     operation_code::SET_LOCAL},
    {"END_LOCAL",     operation_code::END_LOCAL},
    {"GET_UPVALUE",   operation_code::GET_UPVALUE},
    {"SET_UPVALUE",   operation_code::SET_UPVALUE},
    {"GET_PROPERTY",  operation_code::GET_PROPERTY},
    {"SET_PROPERTY",  operation_code::SET_PROPERTY},
    {"GET_SUPER",     operation_code::GET_SUPER},
    {"NOT_EQUAL",     operation_code::NOT_EQUAL},
    {"EQUAL",         operation_code::EQUAL},
    {"GREATER",       operation_code::GREATER},
    {"GREATER_EQUAL", operation_code::GREATER_EQUAL},
    {"LESS",          operation_code::LESS},
    {"LESS_EQUAL",    operation_code::LESS_EQUAL},
    {"ADD",           operation_code::ADD},
    {"SUBTRACT",      operation_code::SUBTRACT},
    {"MULTIPLY",      operation_code::MULTIPLY},
    {"DIVIDE",        operation_code::DIVIDE},
    {"AND",           operation_code::AND},
    {"OR",            operation_code::OR},
    {"NEGATE",        operation_code::NEGATE},
    {"JUMP",          operation_code::JUMP},
    {"JUMP_TO",       operation_code::JUMP_TO},
    {"JUMP_IF_F",     operation_code::JUMP_IF_F},
    {"LOOP",          operation_code::LOOP},
    {"CALL",          operation_code::CALL},
    {"FUNC",          operation_code::FUNC},
    {"INVOKE",        operation_code::INVOKE},
    {"SUPER_INVOKE",  operation_code::SUPER_INVOKE},
    {"CLOSURE",       operation_code::CLOSURE},
    {"CLOSE_UPVALUE", operation_code::CLOSE_UPVALUE},
    {"RETURN",        operation_code::RETURN},
    {"BREAK",         operation_code::BREAK},
    {"CLASS",         operation_code::CLASS},
    {"INHERIT",       operation_code::INHERIT},
    {"METHOD",        operation_code::METHOD},
    {"ILLEGAL",       operation_code::ILLEGAL}
};



struct bytecode{
    operation_code code;
    std::string    value;
};
using bcode = bytecode;

std::string bcode_to_string(bcode);


namespace vm{
    inline std::stack<int> call_stack;
    inline std::stack<environment*> env_stack;

    void run(const std::vector<bcode>&);

    token string_to_token(std::string);
    int split_params(std::string);
}



#endif // __vm_hpp__