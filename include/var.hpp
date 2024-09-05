#ifndef __VAR_HPP__
#define __VAR_HPP__

#include <string>
#include <vector>
#include <map>
#include <set>

#include "token.hpp"
#include "token_type.hpp"
#include "stmt.hpp"

class var{
private:
    token_type type;
    std::string value;
public:
    var();
    var(token_type, std::string);
    token_type  get_type();
    std::string get_value();
};


class func{
private:
    std::vector<token> params;
    stmt body;
    int  defined;
public:
    func();
    func(std::vector<token>, stmt);
    func(std::vector<token>, int);
    int get_arity();
    stmt get_body();
    std::string get_param(int);
    int get_defined();
    bool is_defined();
};


#endif // __VAR_HPP__