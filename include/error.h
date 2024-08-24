#ifndef __ERROR_HEADER__
#define __ERROR_HEADER__

#include <c_types.h>
#include <stdio.h>
#include <stdlib.h>

enum LOX_ERROR{
    SYNTAX_ERROR = 64,
    COMPILE_ERROR,
    RUNTIME_ERROR,
    FILE_NOT_FOUND_ERROR,
    TYPE_ERROR,
    NAME_ERROR,
    ZERO_DIVISION_ERROR,
    IO_ERROR,
};

str error_get_name(enum LOX_ERROR);

// 解析时的错误，如语法错误，带行号
#define raise(l, n, ...) do{\
    fprintf(stderr, "In line %d:\n", l);\
    fprintf(stderr, "[%s]: ", error_get_name(n));\
    fprintf(stderr, __VA_ARGS__);\
    exit(n);\
}while(0)

// 运行时的错误，如除零错误，不带行号
#define throw(n, ...) do{\
    fprintf(stderr, "[%s]: ", error_get_name(n));\
    fprintf(stderr, __VA_ARGS__);\
    exit(n);\
}while(0)

#endif