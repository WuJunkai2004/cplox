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
    var res(
        token_type::NUMBER,
        std::to_string(duration.count())
        );
    ret_stack.set(res);
}

int native::func_clock::build(){
    return 0;
}


void native::func_print::accept(){
    std::cout<<env::get("content")<<std::endl;
}

int native::func_print::build(){
    return 0;
}


void native::func_input::accept(){
    std::string content;
    std::cin>>content;
    ret_stack.set(var(token_type::STRING, content));
}

int native::func_input::build(){
    return 0;
}


void native::func_num::accept(){
    var value = env::get("value");
    double res = 0;
    try{
        res = std::stod(value.get_value());
    } catch(std::invalid_argument){
        ret_stack.set();
        return;
    }
    ret_stack.set(var(token_type::NUMBER, std::to_string(res)));
}

int native::func_num::build(){
    return 0;
}


void native::func_str::accept(){
    var value = env::get("value");
    ret_stack.set(var(token_type::STRING, value.get_value()));
}

int native::func_str::build(){
    return 0;
}


void native::func_exit::accept(){
    var exit_code = env::get("code");
    exit(static_cast<int>(std::stod(exit_code.get_value())));
}

int native::func_exit::build(){
    return 0;
}


void native::func_int::accept(){
    var value = env::get("value");
    int res = 0;
    try{
        res = std::stoi(value.get_value());
    } catch(std::invalid_argument){
        ret_stack.set();
        return;
    }
    ret_stack.set(var(token_type::NUMBER, std::to_string(res)));
}

int native::func_int::build(){
    return 0;
}


void native::func_sleep::accept(){
    var ms = env::get("ms");
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(std::stod(ms.get_value()))));
}

int native::func_sleep::build(){
    return 0; 
}


void native::file_init::accept(){
    env::define("this.path", env::get("path"));
    var mode = env::get("mode");
    env::define("this.mode", mode);
    env::define("this.__inner_mode__", mode);
}

int native::file_init::build(){
    return 0;
}

void native::file_read::accept(){
    var self = this_stack.view_scope();
    std::string name = self.get_value() + ".";
    var path = env::get(name + "path");
    var mode = env::get(name + "__inner_mode__");
    if(mode.get_value() != "r"){
        ret_stack.set();
        return;
    }
    std::ifstream file(path.get_value());
    std::string content;
    std::string line;
    while(std::getline(file, line)){
        content += line + "\n";
    }
    env::assign(name + "__inner_mode__", var(token_type::STRING, "eof"));
    ret_stack.set(var(token_type::STRING, content));
    file.close();
}

int native::file_read::build(){
    return 0;
}


void native::file_write::accept(){
    var self = this_stack.view_scope();
    std::string name = self.get_value() + ".";
    var path = env::get(name + "path");
    var mode = env::get(name + "__inner_mode__");
    if(mode.get_value() != "w" && mode.get_value() != "a"){
        return;
    }
    std::ios_base::openmode open_mode = std::ios::out;
    if(mode.get_value() == "a"){
        open_mode = std::ios::app;
    }
    std::ofstream file(path.get_value(), open_mode);
    var content = env::get("content");
    file<<content.get_value();
    env::assign(name + "__inner_mode__", var(token_type::STRING, "a"));
    file.close();
}

int native::file_write::build(){
    return 0;
}


// 导入原生函数和类
void native::import(){
    register_native();

    // clock函数
    env::func_define(
        "clock",
        std::vector<token>(),
        native_func_register["clock"]
    );

    // print函数
    env::func_define(
        "print",
        std::vector<token>( {token(token_type::IDENTIFIER, "content", "", 0)} ),
        native_func_register["print"]
    );

    // input函数
    env::func_define(
        "input",
        std::vector<token>(),
        native_func_register["input"]
    );

    // num函数
    env::func_define(
        "num",
        std::vector<token>( {token(token_type::IDENTIFIER, "value", "", 0)} ),
        native_func_register["num"]
    );

    // str函数
    env::func_define(
        "str",
        std::vector<token>( {token(token_type::IDENTIFIER, "value", "", 0)} ),
        native_func_register["str"]
    );

    // exit函数
    env::func_define(
        "exit",
        std::vector<token>( {token(token_type::IDENTIFIER, "code", "", 0)} ),
        new func_exit()
    );

    // int函数
    env::func_define(
        "round",
        std::vector<token>( {token(token_type::IDENTIFIER, "value", "", 0)} ),
        native_func_register["round"]
    );

    // sleep函数
    env::func_define(
        "sleep",
        std::vector<token>( {token(token_type::IDENTIFIER, "ms", "", 0)} ),
        native_func_register["sleep"]
    );

    // file类
    std::map<std::string, stmt_method*> file_methods;
    file_methods["init"] = new stmt_method("file.init",
        std::vector<token>({token(token_type::IDENTIFIER, "path", "", 0), 
                            token(token_type::IDENTIFIER, "mode", "", 0)}),
        native_func_register["file.init"]
    );
    file_methods["read"] = new stmt_method("file.read",
        std::vector<token>(),
        native_func_register["file.read"]
    );
    file_methods["write"] = new stmt_method("file.write",
        std::vector<token>({token(token_type::IDENTIFIER, "content", "", 0)}),
        native_func_register["file.write"]
    );
    env::class_define("file", file_methods);
}

// 注册原生函数和类
void native::register_native(){
    native_func_register["clock"] = new func_clock();
    native_func_register["print"] = new func_print();
    native_func_register["input"] = new func_input();
    native_func_register["num"] = new func_num();
    native_func_register["str"] = new func_str();
    native_func_register["exit"] = new func_exit();
    native_func_register["round"] = new func_int();
    native_func_register["sleep"] = new func_sleep();
    native_func_register["file.init"] = new file_init();
    native_func_register["file.read"] = new file_read();
    native_func_register["file.write"] = new file_write();
}