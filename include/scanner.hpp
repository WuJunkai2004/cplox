#pragma once

#ifndef __SCANNER_HPP__
#define __SCANNER_HPP__

#include <string>
#include <vector>

#include "token.hpp"
#include "token_type.hpp"
#include "token.hpp"


class scanner{
private:
    std::string_view source;
    std::vector<token> tokens;

    int start;
    int current;
    int line;

    bool is_at_end();

    char advance();
    bool match_peer(char expected);

    char get_peer();
    char get_next();

    void scan_string(char);
    void scan_number();
    void scan_identifier();

    void scan_token();
    void add_token(token_type type, std::string_view literal = "");
public:
    scanner(std::string_view source);
    std::vector<token> scan_tokens();
};


#endif // __SCANNER_HPP__