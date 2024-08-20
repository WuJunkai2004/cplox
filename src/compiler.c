#include "compiler.h"

#include <c_types.h>

#include "scanner.h"

void COMPILER_compile(str source) {
    list tokens = scanner.scan(source);
    scanner.print(tokens);
    list_free(&tokens);
}

struct __COMPILER__ compiler = {
    .compile = COMPILER_compile
};