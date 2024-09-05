#include "code.hpp"
#include "env.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"
#include "return.hpp"
#include "parser.hpp"
#include "scanner.hpp"
#include "native.hpp"
#include "var.hpp"
#include "vm.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>


std::vector<token> code::generate_tokens(std::string_view source){
    scanner token_scanner(source);
    return token_scanner.scan_tokens();
}


std::vector<stmt> code::generate_ast(std::vector<token> tokens){
    parser token_parser(tokens);
    return token_parser.parse();
}


std::vector<bcode> code::generate_bcode(std::filesystem::path file_path){
    std::ifstream fin(file_path);
    std::string line;
    std::vector<bcode> byte_code_array;
    while(std::getline(fin, line)){
        std::size_t pos = line.find(' ');
        if(pos == std::string::npos){
            byte_code_array.push_back(bcode(
                opcode_map_reverse[line], ""
            ));
        } else {
            byte_code_array.push_back(bcode(
                opcode_map_reverse[line.substr(0, pos)], line.substr(pos+1)
            ));
        }
    }
    return byte_code_array;
}


void code::interpreter::interpret(std::vector<stmt> statements){
    try{
        for(auto& statement : statements){
            execute(statement);
            if(ret_stack.has_set() || brk_stack.has_set()){
                return;
            }
        }
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}


void code::interpreter::execute(stmt statement){
    if(statement == nullptr){
        return;
    }
    statement->accept();
}


var code::interpreter::evaluate(expr expression){
    if(expression == nullptr){
        return var();
    }
    return expression->accept();
}


void code::interpreter::execute_block(std::vector<stmt> statements){
    env::push();
    interpret(statements);
    env::pop();
}


token code::interpreter::call(func function, std::vector<token> arguments){
    env::push();
    ret_stack.into_scope();
    for(int i = 0; i < function.get_arity(); i++){
        env::define(function.get_param(i), arguments[i]);
    }
    execute(function.get_body());
    env::pop();
    return ret_stack.exit_scope();
}


/* =============================== */
/* =============================== */


void code::compiler::compile(std::vector<stmt> statements){
    for(auto& statement : statements){
        statement->build();
    }
}


bcode code::compiler::generate_node(token_type type, std::string value){
    std::map<token_type, operation_code> convert = {
        {token_type::MINUS,         operation_code::SUBTRACT},
        {token_type::PLUS,          operation_code::ADD},
        {token_type::STAR,          operation_code::MULTIPLY},
        {token_type::SLASH,         operation_code::DIVIDE},
        {token_type::GREATER,       operation_code::GREATER},
        {token_type::GREATER_EQUAL, operation_code::GREATER_EQUAL},
        {token_type::LESS,          operation_code::LESS},
        {token_type::LESS_EQUAL,    operation_code::LESS_EQUAL},
        {token_type::EQUAL,         operation_code::SET_ITEM},      //待修改
        {token_type::EQUAL_EQUAL,    operation_code::EQUAL},
        {token_type::BANG_EQUAL,    operation_code::NOT_EQUAL},
        {token_type::AND,           operation_code::AND},
        {token_type::OR,            operation_code::OR},
        {token_type::BANG,          operation_code::NEGATE},
        {token_type::NUMBER,        operation_code::CONSTANT},
        {token_type::STRING,        operation_code::CONSTANT},
        {token_type::NIL,           operation_code::CONSTANT},
        {token_type::TRUE,          operation_code::CONSTANT},
        {token_type::FALSE,         operation_code::CONSTANT},
    };
    if(convert.find(type) != convert.end()){
        return bcode(convert[type], value);
    }
    std::cout<<"[ERROR] Unknown token type: "<< static_cast<int>(type) << std::endl;
    return bcode(operation_code::ILLEGAL, value);
}

bcode code::compiler::generate_node(operation_code op, std::string value){
    return bcode(op, value);
}


int code::compiler::assemble(token_type type, std::string value){
    bcode node = generate_node(type, value);
    byte_code_array.push_back(node);
    return byte_code_array.size() - 1;
}

int code::compiler::assemble(operation_code op, std::string value){
    bcode node = generate_node(op, value);
    byte_code_array.push_back(node);
    return byte_code_array.size() - 1;
}


void code::compiler::modify(int index, token_type type, std::string value){
    bcode node = generate_node(type, value);
    byte_code_array[index] = node;
}

void code::compiler::modify(int index, operation_code op, std::string value){
    bcode node = generate_node(op, value);
    byte_code_array[index] = node;
}

void code::compiler::save(std::filesystem::path save_path){
    std::ofstream file(save_path.stem().string() + ".loxvm", std::ios::out);
    for(const bcode& node : byte_code_array){
        file << bcode_to_string(node) << '\n';
    }
    file.close();
}


/* =============================== */
/* =============================== */


void code::vm::interpret(std::vector<bcode> statements){
    ::vm::run(statements);
}


int code::vm::call(vm_stack& stack, int arity){
    token callee_var = stack.pop();
    std::string callee_name = callee_var.get_literal();
    func  callee_func = env::func_search(callee_name);
    if(callee_func.get_arity() != arity){
        std::cerr << "Function " << callee_name << " expects " << callee_func.get_arity() << " arguments, but got " << arity << std::endl;
        return -1;
    }
    if(native::native_func_register.find(callee_name) != native::native_func_register.end()){
        // native function
        env::push();
        for(int i = arity - 1; i >= 0; i--){
            env::define(callee_func.get_param(i), stack.pop());
        }
        native::native_func_register[callee_name]->accept();
        env::pop();
        return -1;
    }
    return callee_func.get_defined();
}