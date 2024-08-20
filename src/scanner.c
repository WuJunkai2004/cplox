#include "scanner.h"

#include <c_types.h>
#include <stdio.h>

#include "token.h"

static char getEscapedChar(char c){
    switch(c){
        case 'a':
            return '\a';
        case 'n':
            return '\n';
        case 't':
            return '\t';
        case 'r':
            return '\r';
        case '0':
            return '\0';
        case '\\':
            return '\\';
        case '\'':
            return '\'';
        case '"':
            return '"';
        default:
            return 127;
    }
}

static token scan_string(str source, int* current, char quote, int line) {
    int start = *current;
    while(source[*current] != quote && source[*current] != '\0' && source[*current] != '\n'){
        *current += 1;
    }
    if(source[*current] == '\0' || source[*current] == '\n'){
        return token_make(TOKEN_ERROR, line);
    }
    return token_init(
        TOKEN_STRING,
        str_cut(source, start+1, *current - start-1),
        line
    );
}


static token scan_number(str source, int* current, int line) {
    int start = *current;
    while(source[*current] >= '0' && source[*current] <= '9'){
        *current += 1;
    }
    if(source[*current] == '.' && source[*current+1] >= '0' && source[*current+1] <= '9'){
        *current += 1;
        while(source[*current] >= '0' && source[*current] <= '9'){
            *current += 1;
        }
    }
    return token_init(
        TOKEN_NUMBER,
        str_cut(source, start, *current - start),
        line
    );
}

static bool is_keyword(str source, int current, str out){
    for(int i=0; out[i]; i++){
        if(source[current + i] != out[i]){
            return false;
        }
    }
    return true;
}


static token scan_identifier(str source, int* current, int line) {
    int start = *current;
    switch(source[*current-1]){
        case 'a':
            if(is_keyword(source, *current, "nd")){
                *current += 2;
                return token_make(TOKEN_AND, line);
            }
            break;
        case 'b':
            if(is_keyword(source, *current, "reak")){
                *current += 4;
                return token_make(TOKEN_BREAK, line);
            }
            break;
        case 'c':
            if(is_keyword(source, *current, "lass")){
                *current += 4;
                return token_make(TOKEN_CLASS, line);
            }
            break;
        case 'e':
            if(is_keyword(source, *current, "lse")){
                *current += 3;
                return token_make(TOKEN_ELSE, line);
            }
            break;
        case 'f':
            if(is_keyword(source, *current, "alse")){
                *current += 4;
                return token_make(TOKEN_FALSE, line);
            }
            if(is_keyword(source, *current, "or")){
                *current += 2;
                return token_make(TOKEN_FOR, line);
            }
            if(is_keyword(source, *current, "unc")){
                *current += 3;
                return token_make(TOKEN_FUNC, line);
            }
            break;
        case 'i':
            if(is_keyword(source, *current, "f")){
                *current += 1;
                return token_make(TOKEN_IF, line);
            }
            break;
        case 'n':
            if(is_keyword(source, *current, "il")){
                *current += 2;
                return token_make(TOKEN_NIL, line);
            }
            break;
        case 'o':
            if(is_keyword(source, *current, "r")){
                *current += 1;
                return token_make(TOKEN_OR, line);
            }
            break;
        case 'r':
            if(is_keyword(source, *current, "eturn")){
                *current += 5;
                return token_make(TOKEN_RETURN, line);
            }
            break;
        case 's':
            if(is_keyword(source, *current, "uper")){
                *current += 4;
                return token_make(TOKEN_SUPER, line);
            }
            break;
        case 't':
            if(is_keyword(source, *current, "his")){
                *current += 3;
                return token_make(TOKEN_THIS, line);
            }
            if(is_keyword(source, *current, "rue")){
                *current += 3;
                return token_make(TOKEN_TRUE, line);
            }
            break;
        case 'w':
            if(is_keyword(source, *current, "hile")){
                *current += 4;
                return token_make(TOKEN_WHILE, line);
            }
            break;
        case 'p':
            if(is_keyword(source, *current, "rint")){
                *current += 4;
                return token_make(TOKEN_PRINT, line);
            }
            break;
    }
    while(source[*current] >= 'a' && source[*current] <= 'z'){
        *current += 1;
    }
    return token_init(
        TOKEN_IDENTIFIER,
        str_cut(source, start-1, *current - start + 1),
        line
    );
}


static token scan_token(str source, int* current) {
    static int line = 1;
    int cursor = *current;
    *current += 1;
    switch(source[cursor]){
        case '(':
            return token_make(TOKEN_LEFT_PAREN, line);
        case ')':
            return token_make(TOKEN_RIGHT_PAREN, line);
        case '{':
            return token_make(TOKEN_LEFT_BRACE, line);
        case '}':
            return token_make(TOKEN_RIGHT_BRACE, line);
        case ',':
            return token_make(TOKEN_COMMA, line);
        case '.':
            return token_make(TOKEN_DOT, line);
        case '-':
            return token_make(TOKEN_MINUS, line);
        case '+':
            return token_make(TOKEN_PLUS, line);
        case '*':
            return token_make(TOKEN_STAR, line);
        case '/':
            return token_make(TOKEN_SLASH, line);
        case ';':
            return token_make(TOKEN_SEMICOLON, line);
        case '!':
            if(source[*current] == '='){
                *current += 1;
                return token_make(TOKEN_BANG_EQUAL, line);
            }
            return token_make(TOKEN_BANG, line);
        case '=':
            if(source[*current] == '='){
                *current += 1;
                return token_make(TOKEN_EQUAL_EQUAL, line);
            }
            return token_make(TOKEN_EQUAL, line);
        case '<':
            if(source[*current] == '='){
                *current += 1;
                return token_make(TOKEN_LESS_EQUAL, line);
            }
            return token_make(TOKEN_LESS, line);
        case '>':
            if(source[*current] == '='){
                *current += 1;
                return token_make(TOKEN_GREATER_EQUAL, line);
            }
            return token_make(TOKEN_GREATER, line);
        case '\n':
            line += 1;
        case ' ':
        case '\t':
        case '\r':
            break;
        case '"':
        case '\'':
            return scan_string(source, current, source[cursor], line);
        case '0' ... '9':
            return scan_number(source, current, line);
        case 'a' ... 'z':
            return scan_identifier(source, current, line);
        case '\0':
            return token_make(TOKEN_EOF, line);
    }
}


list SCANNER_scan(str source) {
    int current = 0;
    list result = list_create(token);
    while(true){
        token got = scan_token(source, &current);
        list_append(&result, got);
        if(got.type == TOKEN_EOF){
            break;
        }
    }
    return result;
}


void SCANNER_print(list tokens){
    int line=-1;
    for(int i=0; i<tokens.length; i++){
        token got = list_get(token, &tokens, i);
        if(got.line != line){
            printf("%4d ", got.line);
            line = got.line;
        } else {
            printf("   | ");
        }
        token_print(got);
        printf("\n");
    }
}


struct __SCANNER__ scanner = {
    .scan = SCANNER_scan,
    .print = SCANNER_print
};