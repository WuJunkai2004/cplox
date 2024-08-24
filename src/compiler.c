#include "compiler.h"

#include <c_types.h>
#include <stdio.h>

#include "scanner.h"
#include "parser.h"
#include "option.h"
#include "error.h"

struct __COMPILER__ compiler = {
    .compile = COMPILER_compile
};


list COMPILER_compile(str source) {
    list tokens = scanner.scan(source);
    
    if(mode.print_token){
        scanner.print(tokens);
    }

    list bytecode = parser.parse(tokens);
    list_free(&tokens);

    if(mode.release && mode.output){
        FILE* fout = fopen(mode.output, "wb");
        if(!fout){
            throw(IO_ERROR, "cannot open file %s", mode.output);
        }
        fwrite(bytecode._data, sizeof(uint8), bytecode.length, fout);
        fclose(fout);
    }

    return bytecode;
}





