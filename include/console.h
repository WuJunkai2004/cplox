#ifndef __CONSOLE_HEADER__
#define __CONSOLE_HEADER__

#include <c_types.h>

#define MAX_INPUT_BUFFER (256*256)

char getEscapedChar(char);

str  getConsoleInput();

#endif