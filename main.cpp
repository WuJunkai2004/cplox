#include "lox.hpp"
#include "env.hpp"
#include "native.hpp"
#include <iostream>


int main(int argc_len, char* params[]){
    env::init();
    native::import();

    if( argc_len > 2 ){
        std::cerr<<"Usage: cplux [script path]";
        return 64;
    } else if( argc_len == 2 ){
        lox::run_file(params[1]);
    } else {
        lox::run_prompt();
    }
    return 0;
}