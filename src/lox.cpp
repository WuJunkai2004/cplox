#include "lox.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <format>
#include <stack>

#include <string>
#include <vector>

#include "code.hpp"

#include "env.hpp"
#include "error.hpp"

#include "token.hpp"
#include "token_type.hpp"

#include "scanner.hpp"
#include "parser.hpp"


void lox::repl_mode(){
    std::string line;
    int empty_time = 0;

    while(empty_time != 2){
        line = get_prompt();

        if(line.empty()){
            empty_time++;
            continue;
        } else {
            empty_time = 0;
        }

        run(line);
        had_global_error = false;
    }
}


void lox::ast_mode(std::string_view path){
    std::string file_buffer = get_file(path);
    run(file_buffer);
    if(had_global_error){
        exit(65);
    }
    if(had_runtime_error){
        exit(70);
    }
}


void lox::compile_mode(std::string_view path){
    std::string file_buffer = get_file(path);
    std::vector<token> tokens = code::generate_tokens(file_buffer);
    std::vector<stmt>  stmt_list = code::generate_ast(tokens);
    code::compiler::compile(stmt_list);
    code::compiler::save(path);
}


void lox::interpret_mode(std::string_view path){
    std::vector<bcode> byte_code_array = code::generate_bcode(path);
    code::vm::interpret(byte_code_array);
}


lox::file_mode lox::get_file_mode(std::filesystem::path file_path){
    if(file_path.extension() == ".loxvm"){
        return file_mode::COMPILED;
    }
    return file_mode::UNCOMPILED;
}


void lox::run(std::string_view source){
    if(had_global_error){
        return;
    }

    std::vector<token> tokens = code::generate_tokens(source);

    std::vector<stmt> stmt_list;
    try{
        stmt_list = code::generate_ast(tokens);
    } catch(syntax_error& e){
        had_global_error = true;
    }
    
    if(had_global_error){
        return;
    }

    code::interpreter::interpret(stmt_list);
}


int lox::match_prompt(std::string_view line){
    static std::stack<token_type> brace_stack;
    for(const char& local: line){
        switch(local){
            case '(':
                brace_stack.push(token_type::LEFT_PAREN);
                break;
            case ')':
                if(brace_stack.empty() || brace_stack.top() != token_type::LEFT_PAREN){
                    return 2; // error
                }
                brace_stack.pop();
                break;
            case '{':
                brace_stack.push(token_type::LEFT_BRACE);
                break;
            case '}':
                if(brace_stack.empty() || brace_stack.top() != token_type::LEFT_BRACE){
                    return 2; // error
                }
                brace_stack.pop();
                break;
        }
    }
    return brace_stack.empty() ? 0 : 1; // 0: complete, 1: incomplete
}


std::string lox::get_prompt(){
    static int line_number = 0;
    std::string line;
    std::string buffer;

    std::cout<<">>> ";
    line_number++;
    std::getline(std::cin, line);
    buffer.append(line);

    int status = match_prompt(line);

    if(status == 2){
        error(line_number, "Unmatched braces");
        return ";";
    };
    while(status == 1){
        std::cout<<"... ";
        std::getline(std::cin, line);
        buffer.append("\n");
        buffer.append(line);
        status = match_prompt(line);
        if(status == 2){
            error(line_number, "Unmatched braces");
            return ";";
        };
    }
    return buffer;
}


std::string lox::get_file(std::string_view path){
    std::ifstream fin(path.data());
    std::string file_buffer;
    std::string line;
    while(std::getline(fin, line)){
        file_buffer.append(line);
    }
    return file_buffer;
}



void lox::error(int line, std::string_view message){
    report(line, "", message);
}

void lox::report(int line, std::string_view where, std::string_view message){
    std::cerr<<std::format("[line {}] Error {}: {}\n", line, where, message);
    had_global_error = true;
}