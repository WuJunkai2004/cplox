#ifndef __VM_HEADER__
#define __VM_HEADER__

#include <c_types.h>

typedef void* chuck;

int VM_run(chuck, uint32);


enum operate_code{
    OP_PRINT,
    OP_RETURN,
    OP_CONSTANT,        // 2 for constant, 1 for index in the list of data, 2
};




struct __VM__{
    int (*run)(chuck, uint32);
};

extern struct __VM__ vm;

#endif