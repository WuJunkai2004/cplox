#ifndef __FILE_HEADER__
#define __FILE_HEADER__

#include "c_types.h"
#include <stdio.h>

// 获取文件后缀
str_view getFileExtension(str);
// 获取文件地址
str_view getFileDir(str);
// 获取文件名
str_view getFileName(str);

str      getFileContent(str);
str      getFileBinary(str);

// 
FILE*    getMemoryFile(size_t);

size_t   getFileSize(str, str);


#endif