#include <stdio.h>

#include "lox.h"

int main(int argc_len, char* params[]){
    switch(argc_len){
        case 1:
            return lox.repl_run();
        case 2:
            return lox.file_run(params[1]);
        default:
            printf("cplox [file] [option]")
            return 0;
    }
}