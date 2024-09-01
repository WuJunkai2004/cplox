#include "compiler.h"

#include <c_types.h>
#include <stdio.h>

#include "scanner.h"
#include "parser.h"
#include "option.h"
#include "error.h"

struct __COMPILER__ compiler = {
    .compile = COMPILER_compile,
    .save    = COMPILER_save,
};


code COMPILER_generate(str source) {
    list tokens = scanner.scan(source);
    
    if(mode.print_token){
        scanner.print(tokens);
    }

    code result = parser.parse(tokens);
    list_free(&tokens);

    return result;
}

void COMPILER_save(struct code product, str path) {
    FILE* file = fopen(path, "w+");
    if(file == NULL){
        throw(IO_ERROR, "file %s not opened", path);
    }
    // print the header
    fprintf(file, "#include \"lox_native.h\"\n");
    fprintf(file, "#include \"lox_native.c\"\n");
    // print the variable
    for(int i = 0; i < product.global_var.length; i++){
        fprintf(file, "int %s = 0;", list_get(str, &product.global_var, i));
    }
    // print the main function
    fprintf(file, "int main(){\n");
    fprintf(file, "    lox_init();\n");
    fprintf(file, "    // main function\n");
    fprintf(file, "%s", product.main_func);
    fprintf(file, "}\n");
    // close the file
    fclose(file);
}

void COMPILER_compile(str source) {
    // 生成编译命令
    char cmd[1024];
    sprintf(cmd, "gcc %s.c -o %s.exe -O2", source, source);
    system(cmd);
}