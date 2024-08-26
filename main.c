#include <c_types.h>
#include <stdio.h>


#include "file.h"
#include "compiler.h"
#include "option.h"
#include "memory.h"
#include "vm.h"
#include "env.h"

void init(){
    memory.init();
    vm.init();
    env.init();
}

int main(int argc_len, char* params[]){
    init();
    option_parse(argc_len, params);

    if(mode.help){
        printf("Usage: lox [options] [file]\n");
        printf("Options:\n");
        printf("  -d --debug          Run in debug mode\n");
        printf("  -r --release        Run in release mode\n");
        printf("  -h --help           Print this help message\n");
        printf("  -v --version        Print the version\n");
        printf("  --print-token       Print the token\n");
        printf("  --print-compiler    Print the compiler\n");
        return 0;
    }
    if(mode.version){
        printf("Lox version 0.0.1\n");
        return 0;
    }

    str content = getFileContent(mode.input);
    if(content == NULL){
        throw(FILE_NOT_FOUND_ERROR, "file %s not found", mode.input);
    }
    code p = compiler.generate(content);
    compiler.save(p, mode.input);
    compiler.compile(mode.input);
}