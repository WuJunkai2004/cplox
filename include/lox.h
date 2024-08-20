#ifndef __LOX_H__
#define __LOX_H__

#include "c_types.h"

int LOX_repl_run();
int LOX_file_run(str);

int LOX_run(str, int);

void LOX_error(int, str, str);

enum LOX_ERROR{
    SYNTAX_ERROR = 64,
    COMPILE_ERROR,
    RUNTIME_ERROR,
    FILE_NOT_FOUND_ERROR
};

struct __LOX__{
    // 函数指针
    int (*repl_run)();
    int (*file_run)(str);

    void (*error)(int, str, str);
};

extern struct __LOX__ lox;


#endif