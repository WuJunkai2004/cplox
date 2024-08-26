#ifndef __PARSER_HEADER__
#define __PARSER_HEADER__

#include <c_types.h>

#include "compiler.h"

code PARSER_parse(list);


struct __PARSER__{
    code (*parse)(list);
};

extern struct __PARSER__ parser;



#endif