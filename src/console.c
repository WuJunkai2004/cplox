#include "console.h"

#include <C_types.h>

char getEscapedChar(char c){
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


str getConsoleInput(){
    static char buffer[1024 * 8] = {};
    fgets(buffer, 1024 * 8, stdin);
    return buffer;
}