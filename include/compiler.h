#ifndef __COMPILER_HEADER__
#define __COMPILER_HEADER__

#include <c_types.h>

typedef struct {
    list global_var; // list<str>
    list main_func;  // list<char>
} code;

code COMPILER_generate(str);
void     COMPILER_save(code, str);
void     COMPILER_compile(str);


struct __COMPILER__{
    code (*generate)(str);
    void     (*save)(code, str);
    void     (*compile)(str);
};

extern struct __COMPILER__ compiler;

#endif