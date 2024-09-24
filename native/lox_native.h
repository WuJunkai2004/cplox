#ifndef _LOX_NATIVE_H_
#define _LOX_NATIVE_H_

typedef int var;

typedef unsigned char byte;

typedef double number;
typedef char*  string;
typedef int    func;

struct MEMORY {
    byte* data;
    int   idx; // current index
    int   cap; // capacity
    int   max; // max memory size
};

void lox_init();

#define NATIVE_NIL   0
#define NATIVE_FALSE 1
#define NATIVE_TRUE  2

extern struct MEMORY global;
extern struct MEMORY runtime;

enum VAL_TYPE{
    VAL_NIL      = 0b00000010,
    VAL_FALSE    = 0b00000100,
    VAL_TRUE     = 0b00000110,
    VAL_NUMBER   = 0b00001000,
    VAL_STRING   = 0b00001010,
    VAL_FUNC     = 0b00001100,
    VAL_CLASS    = 0b00010000,
    VAL_METHOD   = 0b00010010,
};

int SAVE_CONST_GLOBAL(byte);
int SAVE_NUMBER_GLOBAL(number);
int SAVE_STRING_GLOBAL(string);
int SAVE_FUNC_GLOBAL(func);

int SAVE_NUMBER(number);
int SAVE_STRING(string);
int SAVE_FUNC(func);

#define GARBAGE_SECTION 0b00000001
#define TYPE_SECTION    0b00011110
#define REFER_SECTION   0b11100000

#define RUNTIME_MARK 0b10000000000000000
#define LOCALIZE(i) ({int _i=i;_i & RUNTIME_MARK ? runtime.data + (_i & ~RUNTIME_MARK) : global.data + _i;})

#define GET_TYPE_PTR(ptr) (*((byte*)ptr) & TYPE_SECTION)
#define GET_TYPE(i) GET_TYPE_PTR(LOCALIZE(i))

#define AS_NUMBER_PTR(ptr) (*((number*)(ptr + 1)))
#define AS_STRING_PTR(ptr) ((string)(ptr + 1))
#define AS_FUNC_PTR(ptr)   ((func)(ptr + 1))

#define AS_NUMBER(i) AS_NUMBER_PTR(LOCALIZE(i))
#define AS_STRING(i) AS_STRING_PTR(LOCALIZE(i))
#define AS_FUNC(i)   AS_FUNC_PTR(LOCALIZE(i))

#define MASK_AS_GARBAGE(i) (*((byte*)LOCALIZE(i)) |= GARBAGE_SECTION)

#define REFER_ADD(i) (*((byte*)LOCALIZE(i)) += 0b00100000)
#define REFER_SUB(i) (*((byte*)LOCALIZE(i)) -= 0b00100000)

void garbage_collect();

void raise(string, string);

#define IS_BOOL(a) (GET_TYPE(a) == VAL_TRUE || GET_TYPE(a) == VAL_FALSE)

var adds(var, var);
var subs(var, var);
var muls(var, var);
var divs(var, var);
var negates(var);

var eqs(var, var);  // equal
var neqs(var, var); // not equal
var lts(var, var);  // less than
var les(var, var);  // less or equal
var gts(var, var);  // greater than
var ges(var, var);  // greater or equal

var assign(var, var);

int is_true(var);

string FORMAT(var);


// ***
// *** Native functions
// ***

var __prvar(var);
var __input(var);
var __clock();
var __sleep(var);
var __exit(var);


#endif