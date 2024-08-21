#ifndef __PARSER_HEADER__
#define __PARSER_HEADER__

#include <c_types.h>

list PARSER_parse(list);


struct __PARSER__{
    list (*parse)(list);
    list products;
};

extern struct __PARSER__ parser;



#endif