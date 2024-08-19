#include "vm.hpp"
#include "token.hpp"
#include "token_type.hpp"
#include "env.hpp"
#include "return.hpp"
#include "code.hpp"
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
            case operation_code::SET_ITEM:{
                std::string name = value_stack.pop().get_lexeme();
                env::assign(name, value_stack.pop());
                break;
            }
            case operation_code::SET_VARIABLE:{
                std::string name = value_stack.pop().get_lexeme();
                env::define(name, value_stack.pop());
            }
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
            case operation_code::LESS_EQUAL:{
                token f = value_stack.pop();
                token s = value_stack.pop();
                value_stack.push(f <= s);
                break;
            }
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
                index += std::stoi(codes[index].value);
                break;
            case operation_code::JUMP_IF_F:
                if(!value_stack.pop()){
                    index += std::stoi(codes[index].value);
                }
                break;
            case operation_code::JUMP_TO:
                index = std::stoi(codes[index].value);
                break;
            case operation_code::CALL:
                ret_stack.into_scope();             // 进入函数调用
                call_stack.push(index);             // 保存返回地址
                index = code::vm::call(             // 如果是原生函数，直接调用; 否则返回函数体的起始位置
                    value_stack,
                    std::stoi(codes[index].value)
                );
                if(index == -1){                    // 是原生函数，直接返回
                    index = call_stack.top();       // 恢复返回地址
                    call_stack.pop();               // 弹出返回地址
                    value_stack.push(ret_stack.exit_scope()); // 将返回值压入栈
                    break;
                }
                env_stack.push(env::locale);        // 保存当前环境
                break;
            case operation_code::RETURN:
                index = call_stack.top();
                call_stack.pop();
                ret_stack.set(value_stack.pop());
                while(env::locale != env_stack.top()){
                    env::pop();
                }
                env_stack.pop();
                value_stack.push(ret_stack.exit_scope());
                break;
            case operation_code::FUNC:{
                std::size_t pos = codes[index].value.find(',');
                env::func_define(
                    codes[index].value.substr(0, pos),
                    std::stoi(codes[index].value.substr(pos + 1)),
                    index + 1
                );
            }
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
    return token(token_type::IDENTIFIER, str, str, 0);
}

int vm::split_params(std::string value){ //split by ','
    std::size_t pos = value.find(',');
    return std::stoi(value.substr(pos + 1));
}