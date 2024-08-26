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

    return bytecode;
}
