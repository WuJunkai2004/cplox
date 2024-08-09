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
#include <fstream>
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


void native::file_init::accept(){
    env::define("this.path", env::get_arg("path"));
    token mode = env::get_arg("mode");
    env::define("this.mode", mode);
    env::define("this.__inner_mode__", mode);
}

void native::file_read::accept(){
    token self = this_stack.view_scope();
    std::string name = self.get_lexeme() + ".";
    token path = env::get_arg(name + "path");
    token mode = env::get_arg(name + "__inner_mode__");
    if(mode.get_literal() != "r"){
        ret_stack.set(token(token_type::NIL, "", "", 0));
        return;
    }
    std::ifstream file(path.get_literal());
    std::string content;
    std::string line;
    while(std::getline(file, line)){
        content += line + "\n";
    }
    env::assign(name + "__inner_mode__", token(token_type::STRING, "", "eof", 0));
    ret_stack.set(token(token_type::STRING, "", content, 0));
    file.close();
}


void native::file_write::accept(){
    token self = this_stack.view_scope();
    std::string name = self.get_lexeme() + ".";
    token path = env::get_arg(name + "path");
    token mode = env::get_arg(name + "__inner_mode__");
    if(mode.get_literal() != "w" && mode.get_literal() != "a"){
        return;
    }
    std::ios_base::openmode open_mode = std::ios::out;
    if(mode.get_literal() == "a"){
        open_mode = std::ios::app;
    }
    std::ofstream file(path.get_literal(), open_mode);
    token content = env::get_arg("content");
    file<<content.get_literal();
    env::assign(name + "__inner_mode__", token(token_type::STRING, "", "a", 0));
    file.close();
}


// 导入原生函数和类
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

    // file类
    std::map<std::string, stmt_method*> file_methods;
    file_methods["init"] = new stmt_method("file.init",
        std::vector<token>({token(token_type::IDENTIFIER, "path", "", 0), 
                            token(token_type::IDENTIFIER, "mode", "", 0)}),
        new file_init()
    );
    file_methods["read"] = new stmt_method("file.read",
        std::vector<token>(),
        new file_read()
    );
    file_methods["write"] = new stmt_method("file.write",
        std::vector<token>({token(token_type::IDENTIFIER, "content", "", 0)}),
        new file_write()
    );
    env::class_define("file", file_methods);
}