#include "lox.h"

#include <c_types.h>
#include <stdlib.h>

#include "console.h"
#include "file.h"
#include "compiler.h"

int LOX_repl_run(){
    str line = NULL;
    int line_id = 0;
    while(1){
        printf("lox> ");
        line = getConsoleInput();
        line_id++;
        if(line[0] == 0){
            break;
        }
        LOX_run(line, line_id);
    }
    return 0;
}


int LOX_file_run(str path){
    str content = getFileContent(path);
    if(content == NULL){
        lox.error(-1, "FileNotFoundError", "file not found");
        return FILE_NOT_FOUND_ERROR;
    }
    int status = LOX_run(content, 0);
    free(content);
    return status;
}


int LOX_run(str source, int line_id){
    compiler.compile(source);
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

    .error = LOX_error
};