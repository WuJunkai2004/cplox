#include "lox.h"

#include <c_types.h>
#include <stdlib.h>

#include "console.h"
#include "file.h"
#include "compiler.h"
#include "vm.h"
#include "error.h"


int LOX_file_run(str path){
    str content = getFileContent(path);
    if(content == NULL){
        throw(FILE_NOT_FOUND_ERROR, "file %s not found", path);
    }
    list product = compiler.compile(content);
    free(content);

    int exit_code = vm.run(product._data, product.length);
    return 0;
}


struct __LOX__ lox = {
    .repl_run = LOX_repl_run,
    .file_run = LOX_file_run,
};