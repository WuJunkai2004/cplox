#ifndef __VM_HEADER__
#define __VM_HEADER__

#include <c_types.h>

typedef uint8* chuck;

void VM_init();

int VM_run(chuck, uint32);


enum operate_code{
    OP_END_STMT,
    OP_RETURN,
    OP_CONSTANT,        // 2 for constant, 1 for index in the list of data, 2
    OP_SET_GLOBAL,      // 赋值变量，扫描到的时候定义变量
    OP_GET_GLOBAL,      // 获取变量
    OP_SET_LOCAL,       // 赋值变量，函数内部定义变量
    OP_GET_LOCAL,       // 获取变量
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