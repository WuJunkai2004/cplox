#ifndef __CONSOLE_HEADER__
#define __CONSOLE_HEADER__

#define F_RED(x)    std::format("\033[31m{}\033[0m", x)
#define F_GREEN(x)  std::format("\033[32m{}\033[0m", x)
#define F_YELLOW(x) std::format("\033[33m{}\033[0m", x)

#define F_WHITE_L(x) std::format("\033[37m{}\033[0m", x)

#endif//__CONSOLE_HEADER__