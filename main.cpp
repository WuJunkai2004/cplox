#include "lox.hpp"
#include "env.hpp"
#include "native.hpp"
#include <iostream>


int main(int argc_len, char* params[]){
    env::init();
    native::import();
    #if 1
    if( argc_len == 1 ){
        native::import();
        lox::repl_mode();
        return 0;
    }

    if( argc_len == 2 ){
        lox::file_mode mode = lox::get_file_mode(params[1]);
        if(mode == lox::file_mode::UNCOMPILED){
            lox::ast_mode(params[1]);
        } else {
            lox::interpret_mode(params[1]);
        }
        return 0;
    }

    lox::compile_mode(params[1]);
    return 0;
    #endif
    
    lox::interpret_mode("lox.loxvm");
    return 0;
}