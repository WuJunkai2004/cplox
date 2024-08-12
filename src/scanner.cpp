#include "scanner.hpp"

#include "token.hpp"
#include "token_type.hpp"
#include "lox.hpp"

#include <string>
#include <vector>
#include <map>

#include <cmath>

#include <iostream>

scanner::scanner(std::string_view source_):
    source(source_), tokens() {
    start = 0;
    current = 0;
    line = 1;
}


std::vector<token> scanner::scan_tokens(){
    while(!is_at_end()){
        start = current;
        scan_token();
    }
    tokens.emplace_back(
        token(token_type::END, "", "", line)
    );
    return tokens;
}


bool scanner::is_at_end(){
    return current >= source.size();
}


char scanner::advance(){
    current++;
    return source[current - 1];
}


bool scanner::match_peer(char expected){
    if(is_at_end()){
        return false;
    }
    if(source[current] != expected){
        return false;
    }
    current++;
    return true;
}

char scanner::get_peer(){
    if(is_at_end()){
        return '\0';
    }
    return source[current];
}

char scanner::get_next(){
    if(current + 1 >= source.size()){
        return '\0';
    }
    return source[current + 1];
}


void scanner::scan_string(char end_char){
    std::map<char, char> escape_chars = {
        {'n', '\n'},
        {'r', '\r'},
        {'t', '\t'},
        {'\'', '\''},
        {'"', '"'},
        {'\\', '\\'},
        {'0', '\0'},
        {'\n', '\n'}
    };
    std::string str;
    while(get_peer() != end_char && !is_at_end()){
        if(get_peer() == '\n'){
            line++;
        }
        char get_ch = advance();
        if(get_ch == '\\'){
            if(is_at_end()){
                lox::error(line, "Unterminated string.");
                return;
            }
            char escape_ch = advance();
            if(escape_chars.find(escape_ch) != escape_chars.end()){
                str.push_back(escape_chars[escape_ch]);
            } else {
                lox::error(line, "Invalid escape character.");
                return;
            }
        } else [[likely]] {
            str.push_back(get_ch);
        }
    }
    if(is_at_end()){
        lox::error(line, "Unterminated string.");
        return;
    }
    advance();
    add_token(token_type::STRING, str);
}


void scanner::scan_number(){
    while(std::isdigit(get_peer())){
        advance();
    }
    if(get_peer() == '.' && std::isdigit(get_next())){
        advance();
        while(std::isdigit(get_peer())){
            advance();
        }
    }
    add_token(
        token_type::NUMBER, 
        source.substr(start, current - start)
    );
}

void scanner::scan_identifier(){
    while(std::isalnum(get_peer()) || get_peer() == '_'){
        advance();
    }
    std::string text(source.substr(start, current - start));
    token_type type = token_type::IDENTIFIER;
    if(keywords.find(text) != keywords.end()){
        type = keywords[text];
    }
    add_token(type);
}


void scanner::add_token(token_type type, std::string_view literal){
    std::string_view text = source.substr(start, current - start);
    tokens.emplace_back(
        token(type, text, literal, line)
    );
}


void scanner::scan_token(){
    char c = advance();
    switch(c){
        case '(': 
            add_token(token_type::LEFT_PAREN);
            break;
        case ')':
            add_token(token_type::RIGHT_PAREN);
            break;
        case '{':
            add_token(token_type::LEFT_BRACE);
            break;
        case '}':
            add_token(token_type::RIGHT_BRACE);
            break;
        case ',':
            add_token(token_type::COMMA);
            break;
        case '.':
            add_token(token_type::DOT);
            break;
        case '-':
            add_token(token_type::MINUS);
            break;
        case '+':
            add_token(token_type::PLUS);
            break;
        case ';':
            add_token(token_type::SEMICOLON);
            break;
        case '*':
            add_token(token_type::STAR);
            break;
        case '!':
            add_token(
                match_peer('=') ? token_type::BANG_EQUAL : token_type::BANG
            );
            break;
        case '=':
            add_token(
                match_peer('=') ? token_type::EQUAL_EQUAL : token_type::EQUAL
            );
            break;
        case '<':
            add_token(
                match_peer('=') ? token_type::LESS_EQUAL : token_type::LESS
            );
            break;
        case '>':
            add_token(
                match_peer('=') ? token_type::GREATER_EQUAL : token_type::GREATER
            );
            break;
        case '/':
            if(match_peer('/')){
                while(get_peer() != '\n' && !is_at_end()){
                    advance();
                }
            } else {
                add_token(token_type::SLASH);
            }
            break;
        case ' ':
            [[fallthrough]];
        case '\r':
            [[fallthrough]];
        case '\t':
            break;
        case '\n':
            line++;
            break;
        case '"':
        case '\'':
            scan_string(c);
            break;
        case '0' ... '9':
            scan_number();
            break;
        case 'a' ... 'z':
            [[fallthrough]];
        case 'A' ... 'Z':
            [[fallthrough]];
        case '_':
            scan_identifier();
            break;
        default:
            lox::error(line, "Unexpected character.");
            break;
    }
}