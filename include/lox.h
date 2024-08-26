#ifndef __LOX_H__
#define __LOX_H__

#include "c_types.h"

int LOX_repl_run();
int LOX_file_run(str);
int LOX_byte_run(str);

struct __LOX__{
    /**
     * @brief Run the REPL mode
     */
    int (*repl_run)();

    /**
     * @brief Run the file mode
     */
    int (*file_run)(str);
};

extern struct __LOX__ lox;


#endif