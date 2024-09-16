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

std::string lox::load_file(std::string_view path){
    std::ifstream fin(path.data());
    std::string file_buffer;
    std::string line;
    while(std::getline(fin, line)){
        file_buffer.append(line);
    }
    return file_buffer;
}

std::string lox::save_file(std::string_view path, std::string_view content){
    std::ofstream fout(path.data());
    fout.write(content.data(), content.size());
    return path.data();
}


std::string lox::translate(std::string_view path){
    std::string source = load_file(path);
    std::vector<token> tokens    = generate_tokens(source);
    std::vector<stmt>  stmt_list = generate_stmt(tokens);
    return code::translate(stmt_list);
}


void lox::compile(std::string_view path){
    return;
}


std::vector<token> lox::generate_tokens(std::string_view source){
    scanner token_scanner(source);
    return token_scanner.scan_tokens();
}


std::vector<stmt> lox::generate_stmt(std::vector<token>& tokens){
    parser token_parser(tokens);
    return token_parser.parse();
}

void lox::error(int line, std::string_view message){
    report(line, "", message);
}

void lox::report(int line, std::string_view where, std::string_view message){
    std::cerr<<std::format("[line {}] Error {}: {}\n", line, where, message);
    had_global_error = true;
}