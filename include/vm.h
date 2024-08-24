#ifndef __VM_HEADER__
#define __VM_HEADER__

#include <c_types.h>

typedef uint8* chuck;

void VM_init();

int VM_run(chuck, uint32);


enum operate_code{
    OP_PRINT,
    OP_RETURN,
    OP_CONSTANT,        // 2 for constant, 1 for index in the list of data, 2
    OP_SET_VAR,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NEGATE,
};




struct __VM__{
    void (*init)();
    int (*run)(chuck, uint32);
};

extern struct __VM__ vm;

#endif