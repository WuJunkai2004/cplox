#include "lox.hpp"

#include <iostream>
#include <fstream>
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


void lox::run(std::string_view source){
    if(had_global_error){
        return;
    }

    scanner token_scanner(source);
    std::vector<token> tokens = token_scanner.scan_tokens();

    parser token_parser(tokens);
    std::vector<stmt> stmt_list;
    try{
        stmt_list = token_parser.parse();
    } catch(syntax_error& e){
        std::cerr << e.what() << std::endl;
        had_global_error = true;
    }
    
    if(had_global_error){
        return;
    }

    code::interpret(stmt_list);
}


void lox::run_file(std::string_view path){
    std::ifstream fin(path.data());
    std::string file_buffer;
    std::string line;
    while(std::getline(fin, line)){
        file_buffer.append(line);
    }
    run(file_buffer);
    if(had_global_error){
        exit(65);
    }
    if(had_runtime_error){
        exit(70);
    }
}

void lox::run_prompt(){
    std::string line;
    int empty_time = 0;

    while(empty_time != 2){
        line = get_prompt();

        if(line == "pop"){
            std::cout<<"Popping environment\n";
            env::pop();
            continue;
        }
        if(line == "push"){
            std::cout<<"Pushing environment\n";
            env::push();
            continue;
        }
        if(line.substr(0, 4) == "list"){
            std::string var_name = line.substr(5);
            func f = env::func_search(var_name);
            std::cout<<"Function "<<var_name<<" defined: "<<f.is_defined()<<"\n";
            std::cout<<"Function arity: "<<f.get_arity()<<"\n";
            continue;
        }
        
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



void lox::error(int line, std::string_view message){
    report(line, "", message);
}

void lox::report(int line, std::string_view where, std::string_view message){
    std::cerr<<std::format("[line {}] Error {}: {}\n", line, where, message);
    had_global_error = true;
}