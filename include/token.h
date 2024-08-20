#ifndef __TOKEN_HEADER__
#define __TOKEN_HEADER__

#include <c_types.h>

enum token_type{
    // one character tokens
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
    TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
    TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,

    // one or two character tokens
    TOKEN_BANG,     TOKEN_BANG_EQUAL,
    TOKEN_EQUAL,    TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER,  TOKEN_GREATER_EQUAL,
    TOKEN_LESS,     TOKEN_LESS_EQUAL,

    // literals
    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,
    TOKEN_TRUE, TOKEN_FALSE, TOKEN_NIL,

    // keywords
    TOKEN_AND, TOKEN_OR,
    TOKEN_IF, TOKEN_ELSE,
    TOKEN_FOR, TOKEN_WHILE,
    TOKEN_BREAK,
    TOKEN_FUNC, TOKEN_RETURN,
    TOKEN_CLASS, TOKEN_THIS, TOKEN_SUPER,

    // error
    TOKEN_ERROR,

    // end of file
    TOKEN_EOF,

    // PRINT FOR TEMPORARY USE
    TOKEN_PRINT
};


typedef struct {
    enum token_type type;
    str_view        lexeme;
    int             line;
} token;

token token_make(enum token_type, int);
token token_init(enum token_type, str_view, int);
void  token_print(token);

#endif