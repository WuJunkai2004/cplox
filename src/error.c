#include "error.h"

#include <c_types.h>

str error_get_name(enum LOX_ERROR error){
    switch(error){
        case SYNTAX_ERROR:
            return "SyntaxError";
        case COMPILE_ERROR:
            return "CompileError";
        case RUNTIME_ERROR:
            return "RuntimeError";
        case FILE_NOT_FOUND_ERROR:
            return "FileNotFoundError";
        case TYPE_ERROR:
            return "TypeError";
        case NAME_ERROR:
            return "NameError";
        case ZERO_DIVISION_ERROR:
            return "ZeroDivisionError";
        case IO_ERROR:
            return "IOError";
        default:
            return "UnknownError";
    }
}