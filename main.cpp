#include "lox.hpp"
#include "env.hpp"
#include "native.hpp"
#include "console.hpp"
#include <iostream>
#include <format>


int main(int argc_len, char* params[]){
    env::init();
    native::import();

    if( argc_len == 1 ){
        std::cout<<F_WHITE_L("lox: ")<<F_RED("fatal error: ")<<"no input files"<<std::endl;
        std::cout<<"compilation terminated."<<std::endl;
        return 1;
    }

    std::string code = lox::translate(params[1]);

    std::string file = lox::save_file("lox.c", code);

    lox::compile(file);
}