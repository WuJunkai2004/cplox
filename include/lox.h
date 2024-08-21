#ifndef __LOX_H__
#define __LOX_H__

#include "c_types.h"

int LOX_repl_run();
int LOX_file_run(str);
int LOX_byte_run(str);

void LOX_error(int, str, str);

enum LOX_ERROR{
    SYNTAX_ERROR = 64,
    COMPILE_ERROR,
    RUNTIME_ERROR,
    FILE_NOT_FOUND_ERROR
};

struct __LOX__{
    /**
     * @brief Run the REPL mode
     */
    int (*repl_run)();

    /**
     * @brief Run the file mode
     */
    int (*file_run)(str);

    /**
     * @brief Run the bytecode
     */
    int (*byte_run)(str);

    /**
     * @brief Report an error
     */
    void (*error)(int, str, str);
};

extern struct __LOX__ lox;


#endif