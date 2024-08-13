#include "vm.hpp"
#include "token.hpp"
#include "token_type.hpp"
#include "env.hpp"
#include "return.hpp"
#include <string>
#include <stack>
#include <vector>


std::string bcode_to_string(bcode bc){
    std::string res = opcode_map[bc.code];
    if(bc.value != ""){
        res += " " + bc.value;
    }
    return res;
}


void vm::run(const std::vector<bcode>& codes){
    int index = 0;
    while(index < codes.size()){
        switch(codes[index].code){
            case operation_code::CONSTANT:
                value_stack.push(string_to_token(codes[index].value));
                break;
            case operation_code::NIL:
                value_stack.push(token(token_type::NIL, "nil", "nil", 0));
                break;
            case operation_code::TRUE:
                value_stack.push(token(token_type::TRUE, "true", "true", 0));
                break;
            case operation_code::FALSE:
                value_stack.push(token(token_type::FALSE, "false", "false", 0));
                break;
            case operation_code::POP:
                value_stack.pop();
                break;
            case operation_code::GET_ITEM:
                value_stack.push( env::get_arg(codes[index].value) );
                break;
            case operation_code::SET_ITEM:
                env::define(codes[index].value, value_stack.top());
                break;
            case operation_code::SET_LOCAL:
                env::push();
                break;
            case operation_code::END_LOCAL:
                env::pop();
                break;
            case operation_code::NOT_EQUAL:
                value_stack.push(value_stack.pop() != value_stack.pop());
                break;
            case operation_code::EQUAL:
                value_stack.push(value_stack.pop() == value_stack.pop());
                break;
            case operation_code::GREATER:
                value_stack.push(value_stack.pop() > value_stack.pop());
                break;
            case operation_code::GREATER_EQUAL:
                value_stack.push(value_stack.pop() >= value_stack.pop());
                break;
            case operation_code::LESS:
                value_stack.push(value_stack.pop() < value_stack.pop());
                break;
            case operation_code::LESS_EQUAL:
                value_stack.push(value_stack.pop() <= value_stack.pop());
                break;
            case operation_code::ADD:
                value_stack.push(value_stack.pop() + value_stack.pop());
                break;
            case operation_code::SUBTRACT:
                value_stack.push(value_stack.pop() - value_stack.pop());
                break;
            case operation_code::MULTIPLY:
                value_stack.push(value_stack.pop() * value_stack.pop());
                break;
            case operation_code::DIVIDE:
                value_stack.push(value_stack.pop() / value_stack.pop());
                break;
            case operation_code::NEGATE:{
                token p = value_stack.pop();
                token n(p? token_type::FALSE: token_type::TRUE, "", "", p.get_line());
                value_stack.push(n);
                break;
            }
            case operation_code::JUMP:
                index = std::stoi(codes[index].value);
                break;
            case operation_code::JUMP_IF_F:
                if(!value_stack.pop()){
                    index = std::stoi(codes[index].value);
                }
                break;
            default:
                break;

        }
        index++;
    }
}


token vm::string_to_token(std::string str){
    //true, false, nil
    if(str == "true"){
        return token(token_type::TRUE, "true", "true", 0);
    } else if(str == "false"){
        return token(token_type::FALSE, "false", "false", 0);
    } else if(str == "nil"){
        return token(token_type::NIL, "nil", "nil", 0);
    }
    if(str[0] == '"'){
        std::string literal = str.substr(1, str.size() - 2);
        return token(token_type::STRING, str, literal, 0);
    }
    bool is_number = true;
    int dot_count = 0;
    for(auto c : str){
        if(!isdigit(c)){
            if(c == '.'){
                dot_count++;
            }else{
                is_number = false;
                break;
            }
        }
    }
    if(is_number && dot_count <= 1){
        return token(token_type::NUMBER, str, str, 0);
    }
    return token(token_type::NIL, "nil", "nil", 0);
}

std::string vm::token_to_string(token t){
    switch(t.get_type()){
        case token_type::TRUE:
            return "true";
        case token_type::FALSE:
            return "false";
        case token_type::NIL:
            return "nil";
        case token_type::STRING:{
            std::string res="\"";
            for(auto c : t.get_literal()){
                switch(c){
                    case '\n':
                        res += "\\n";
                        break;
                    case '\r':
                        res += "\\r";
                        break;
                    case '\t':
                        res += "\\t";
                        break;
                    case '\\':
                        res += "\\\\";
                        break;
                    case '"':
                        res += "\\\"";
                        break;
                    default:
                        res += c;
                        break;
                }
            }
            res += "\"";
            return res;
        }
        case token_type::NUMBER:
            return t.get_literal();
        default:
            return "nil";
    }
}