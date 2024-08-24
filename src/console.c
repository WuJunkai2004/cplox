#include "console.h"

#include <c_types.h>

#include <stdio.h>
#include <string.h>


str getConsoleInput(){
    static char buffer[MAX_INPUT_BUFFER] = {};
    static str  input = buffer;
    fgets(input, MAX_INPUT_BUFFER - (input - buffer), stdin);
    int len = strlen(input);
    if(input[len - 1] == '\n'){
        input[len - 1] = '\0';
    }
    input += len;
    return input - len;
}