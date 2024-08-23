#ifndef __VAR_HEADER__
#define __VAR_HEADER__

#include <c_types.h>

typedef struct{
    uint16 deepth;
    uint32 offset;
} var;

typedef struct{
    str_view name;
    var      data;
} hash_var;

#define VAR(d, o) ((var){.deepth = d, .offset = o})


#endif