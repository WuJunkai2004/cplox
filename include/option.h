#ifndef __OPTION_HEADER__
#define __OPTION_HEADER__

#include <c_types.h>

struct __MODE__ {
    bool repl;          // default mode

    bool debug;         // -d --debug
    bool release;       // -r --release
    str  input;         // -i --input
    str  output;        // -o --output

    bool help;          // -h --help
    bool version;       // -v --version
    
    bool print_token;   // --print-token
    bool print_compiler;// --print-compiler
};

extern struct __MODE__ mode;

void option_parse(int, str argv[]);


#endif