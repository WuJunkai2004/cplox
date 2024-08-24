#include "lox.h"

#include <c_types.h>
#include <stdlib.h>

#include "console.h"
#include "file.h"
#include "compiler.h"
#include "vm.h"
#include "error.h"

int LOX_repl_run(){
    str line = NULL;
    while(1){
        printf("lox> ");
        line = getConsoleInput();
        if(line[0] == 0){
            break;
        }
        list bytecode = compiler.compile(line);
        vm.run(bytecode._data, bytecode.length);
    }
    return 0;
}


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


int LOX_byte_run(str path){
    str content = getFileBinary(path);
    size_t limit = getFileSize(path, "rb");
    if(content == NULL){
        throw(FILE_NOT_FOUND_ERROR, "file %s not found", path);
    }
    int status = vm.run(content, limit);
    free(content);
    return status;
}


struct __LOX__ lox = {
    .repl_run = LOX_repl_run,
    .file_run = LOX_file_run,
    .byte_run = LOX_byte_run,
};