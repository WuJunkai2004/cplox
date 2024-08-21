#ifndef __COMPILER_HEADER__
#define __COMPILER_HEADER__

#include <c_types.h>

list COMPILER_compile(str source);


struct __COMPILER__{
    list (*compile)(str);
};

extern struct __COMPILER__ compiler;

#endif