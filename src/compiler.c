#include "compiler.h"

#include <c_types.h>
#include <stdio.h>

#include "scanner.h"
#include "parser.h"
#include "option.h"

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
            printf("Error: cannot open file %s\n", mode.output);
            return list_create_by(0);
        }
        fwrite(bytecode._data, sizeof(uint8), bytecode.length, fout);
        fclose(fout);
    }

    return bytecode;
}





