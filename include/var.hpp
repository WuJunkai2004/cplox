#ifndef __VAR_HPP__
#define __VAR_HPP__

#include <string>
#include <vector>
#include <map>
#include <set>

#include "token.hpp"
#include "token_type.hpp"


class var{
private:
    token_type type;
    std::string value;
public:
    var();
    var(token_type, std::string);
    token_type  get_type();
    std::string get_value();

    var operator+(var);
    var operator-(var);
    var operator*(var);
    var operator/(var);

    var operator==(var);
    var operator!=(var);
    var operator< (var);
    var operator> (var);
    var operator<=(var);
    var operator>=(var);

    operator bool();

    friend bool is_same_type(const var*, const var*);
    friend bool is_bool_type(const var*);
    friend bool is_comparable(const var*, const var*);

    friend std::ostream& operator<<(std::ostream&, const var);
};


bool is_same_type(const var*, const var*);
bool is_bool_type(const var*);
bool is_comparable(const var*, const var*);


class func{
private:
    std::vector<token> params;
    void* body;
    int  defined;
public:
    func();
    func(std::vector<token>, void*);
    func(std::vector<token>, int);
    int get_arity();
    void* get_body();
    std::string get_param(int);
    int get_defined();
    bool is_defined();
};


#endif // __VAR_HPP__