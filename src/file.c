#include "file.h"

#include <C_types.h>
#include <stdio.h>

str_view getFileExtension(str path){
    int len = strlen(path);
    for(int i=len-1; i>=0; i--){
        if(path[i] == '.'  ||
           path[i] == '\\' ||
           path[i] == '/'
           ){
            return str_cut(path, i, len-i);
        }
    }
    return str_cut(path, len, 0);
}


str getFileContent(str path){
    size_t size = getFileSize(path);
    str buffer = str_new(size);
    FILE* file = fopen(path, "r");
    if(file == NULL){
        return NULL;
    }
    fread(buffer, 1, size, file);
    fclose(file);
    return buffer;
}


size_t getFileSize(str path){
    FILE* file = fopen(path, "r");
    if(file == NULL){
        return 0;
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fclose(file);
    return size;
}