#include "lox.hpp"
#include "env.hpp"
#include "native.hpp"
#include <iostream>


int main(int argc_len, char* params[]){
    env::init();

    if( argc_len == 1 ){
        native::import();
        lox::repl_mode();
        return 0;
    }

    if( argc_len == 2 ){
        lox::file_mode mode = lox::get_file_mode(params[1]);
        if(mode == lox::file_mode::UNCOMPILED){
            native::import();
            lox::ast_mode(params[1]);
        } else {
        //  native::regist()
            lox::interpret_mode(params[1]);
        }
        return 0;
    }

    // native::regist();
    lox::compile_mode(params[1]);
    return 0;
}