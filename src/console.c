#include "console.h"

#include <c_types.h>

#include <stdio.h>


str getConsoleInput(){
    static char buffer[1024 * 8] = {};
    fgets(buffer, 1024 * 8, stdin);
    return buffer;
}