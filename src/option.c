#include "option.h"

#include <c_types.h>
#include <string.h>
#include <stdlib.h>

struct __MODE__ mode = {
    .debug = false,
    .release = false,
    .input = NULL,
    .help = false,
    .version = false,
    .print_token = false,
    .print_compiler = false
};

void option_parse(int argv_len, str argv[]){
    if(argv_len <= 1){
        mode.repl = true;
        return;
    }
    if(argv[1][0] != '-'){
        mode.input = argv[1];
    }
    for(int i=2; i<argv_len; i++){
        if(argv[i][0] == '-'){
            if(strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0){
                mode.debug = true;
            }else if(strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--release") == 0){
                mode.release = true;
            }else if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
                mode.help = true;
            }else if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0){
                mode.version = true;
            }else if(strcmp(argv[i], "--print-token") == 0){
                mode.print_token = true;
            }else if(strcmp(argv[i], "--print-compiler") == 0){
                mode.print_compiler = true;
            }
        }
    }
}