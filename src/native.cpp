#include "native.hpp"
#include "token.hpp"
#include "token_type.hpp"
#include "stmt.hpp"
#include "env.hpp"
#include "return.hpp"

#include <vector>
#include <string>
#include <chrono>
#include <iostream>
#include <thread>


native::func_clock::func_clock(){
    start = std::chrono::system_clock::now();
}

void native::func_clock::accept(){
    time_type end = std::chrono::system_clock::now();
    dura_type duration = end - start;
    token res(
        token_type::NUMBER,
        "",
        std::to_string(duration.count()),
        0);
    ret_stack.set(res);
}


void native::func_print::accept(){
    std::cout<<env::get_arg("content")<<std::endl;
}


void native::func_input::accept(){
    std::string content;
    std::cin>>content;
    ret_stack.set(token(token_type::STRING, "", content, 0));
}


void native::func_num::accept(){
    token value = env::get_arg("value");
    double res = 0;
    try{
        res = std::stod(value.get_literal());
    } catch(std::invalid_argument){
        ret_stack.set(token(token_type::NIL, "", "", 0));
        return;
    }
    ret_stack.set(token(token_type::NUMBER, "", std::to_string(res), 0));
}


void native::func_str::accept(){
    token value = env::get_arg("value");
    ret_stack.set(token(token_type::STRING, "", value.get_literal(), 0));
}


void native::func_exit::accept(){
    token code = env::get_arg("code");
    exit(static_cast<int>(std::stod(code.get_literal())));
}


void native::func_int::accept(){
    token value = env::get_arg("value");
    int res = 0;
    try{
        res = std::stoi(value.get_literal());
    } catch(std::invalid_argument){
        ret_stack.set(token(token_type::NIL, "", "", 0));
        return;
    }
    ret_stack.set(token(token_type::NUMBER, "", std::to_string(res), 0));
}


void native::func_sleep::accept(){
    token value = env::get_arg("ms");
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(std::stod(value.get_literal()))));
}



// 导入原生函数
void native::import(){
    // clock函数
    env::func_define(
        "clock",
        std::vector<token>(),
        new func_clock()
    );

    // print函数
    env::func_define(
        "print",
        std::vector<token>( {token(token_type::IDENTIFIER, "content", "", 0)} ),
        new func_print()
    );

    // input函数
    env::func_define(
        "input",
        std::vector<token>(),
        new func_input()
    );

    // num函数
    env::func_define(
        "num",
        std::vector<token>( {token(token_type::IDENTIFIER, "value", "", 0)} ),
        new func_num()
    );

    // str函数
    env::func_define(
        "str",
        std::vector<token>( {token(token_type::IDENTIFIER, "value", "", 0)} ),
        new func_str()
    );

    // exit函数
    env::func_define(
        "exit",
        std::vector<token>( {token(token_type::IDENTIFIER, "code", "", 0)} ),
        new func_exit()
    );

    // int函数
    env::func_define(
        "int",
        std::vector<token>( {token(token_type::IDENTIFIER, "value", "", 0)} ),
        new func_int()
    );

    // sleep函数
    env::func_define(
        "sleep",
        std::vector<token>( {token(token_type::IDENTIFIER, "ms", "", 0)} ),
        new func_sleep()
    );
}