#include "parser.h"

#include <c_types.h>

#include "token.h"

struct __PARSER__ parser = {
    .parse = PARSER_parse,
    .products = {}
};


/**
 * @brief parse the token list to bytecode
 * @param tokens: the token list
 * @return the bytecode list
*/
list PARSER_parse(list tokens){
    parser.products = list_create(uint8);
    for(int idx = 0; idx < tokens.length; idx++){
        token local = list_get(token, &tokens, idx);
        switch(local.type){
            case TOKEN_VAR:
                break;
            case TOKEN_FUNC:
                break;
            case TOKEN_IF:
                break;
            default:
                break;
        }
    }
    return parser.products;
}