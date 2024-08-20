#ifndef __COMPILER_HEADER__
#define __COMPILER_HEADER__

#include <c_types.h>

void COMPILER_compile(str source);


struct __COMPILER__{
    void (*compile)(str);
};

extern struct __COMPILER__ compiler;


#endif