#include <c_types.h>
#include <stdio.h>

#include "lox.h"
#include "file.h"
#include "option.h"
#include "memory.h"
#include "vm.h"

int main(int argc_len, char* params[]){
    memory.init();
    vm.init();
    option_parse(argc_len, params);

    // run the REPL mode from the command line
    if(mode.repl){
        return lox.repl_run();
    }
    return lox.repl_run();
    // run the bytecode and file mode
    if( view_to_str(getFileExtension(mode.input)) == string("loxbc") ){
        return lox.byte_run(mode.input);
    } else {
        return lox.file_run(mode.input);
    }

}