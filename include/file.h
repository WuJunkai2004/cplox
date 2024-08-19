#ifndef __FILE_HEADER__
#define __FILE_HEADER__

#include "C_types.h"
#include <stdio.h>

// 获取文件后缀
str_view getFileExtension(str_view);

str      getFileContent(str);

size_t   getFileSize(str);


#endif