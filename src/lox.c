#include "lox.h"

#include <c_types.h>
#include <stdlib.h>

#include "console.h"
#include "file.h"
#include "compiler.h"
#include "vm.h"

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
        lox.error(-1, "FileNotFoundError", "file not found");
        return FILE_NOT_FOUND_ERROR;
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
        lox.error(-1, "FileNotFoundError", "file not found");
        return FILE_NOT_FOUND_ERROR;
    }
    int status = vm.run(content, limit);
    free(content);
    return status;
}


void LOX_error(int error_line, str error_type, str error_message){
    if(error_line == -1){
        printf("[%s]: %s\n", error_type, error_message);
    } else {
        printf("In line %d\n", error_line);
        printf("\t[%s]: %s\n", error_type, error_message);
    }
}


struct __LOX__ lox = {
    .repl_run = LOX_repl_run,
    .file_run = LOX_file_run,
    .byte_run = LOX_byte_run,

    .error = LOX_error
};