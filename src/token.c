#include "token.h"

#include <c_types.h>
#include <stdio.h>

token token_make(enum token_type type, int line) {
    token ret = {
        .type = type,
        .line = line,
        .lexeme = {
            .start = NULL,
            .len = 0
        }
    };
    return ret;
}


token token_init(enum token_type type, str_view lexeme, int line) {
    token ret = {
        .type = type,
        .line = line,
        .lexeme = lexeme
    };
    return ret;
}


void token_print(token tok) {
    printf("%04d ", tok.type);
    for(int i = 0; i < tok.lexeme.len; i++) {
        printf("%c", tok.lexeme.start[i]);
    }
}