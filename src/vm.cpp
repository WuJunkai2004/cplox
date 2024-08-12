#include "vm.hpp"

operation_code get_operation_code(int code){
    //先排除掉不在范围内的操作码
    if(code < 0 || code >= static_cast<int>(operation_code::ILLEGAL)){
        return operation_code::ILLEGAL;
    }
    return static_cast<operation_code>(code);
}