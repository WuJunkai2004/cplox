#include <c_types.h>
#include <stdio.h>

#include "lox.h"
#include "file.h"
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

    // run the REPL mode from the command line
    if(mode.repl){
        return lox.repl_run();
    }
    return lox.file_run(mode.input);
}