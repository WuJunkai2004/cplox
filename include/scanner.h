#ifndef __SCANNER_HEADER__
#define __SCANNER_HEADER__

#include <c_types.h>

// list<token>
list SCANNER_scan(str);

void SCANNER_print(list);

struct __SCANNER__{
    list (*scan)(str);
    void (*print)(list);
};

extern struct __SCANNER__ scanner;

#endif