#ifndef _LOX_NATIVE_H_
#define _LOX_NATIVE_H_

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

int SAVE_CONST(byte);
int SAVE_NUMBER(number);
int SAVE_STRING(string);
int SAVE_FUNC(func);

#define GET_TYPE_PTR(ptr) (*((byte*)ptr) & 0b00011110)
#define GET_TYPE(i) GET_TYPE_PTR(LOCALIZE(i))

#define RUNTIME_MARK 0b10000000000000000
#define LOCALIZE(i) ({int _i=i;_i & RUNTIME_MARK ? runtime.data + (_i & ~RUNTIME_MARK) : global.data + _i;})

#define AS_NUMBER_PTR(ptr) (*((number*)(ptr + 1)))
#define AS_STRING_PTR(ptr) ((string)(ptr + 1))
#define AS_FUNC_PTR(ptr)   ((func)(ptr + 1))

#define AS_NUMBER(i) AS_NUMBER_PTR(LOCALIZE(i))
#define AS_STRING(i) AS_STRING_PTR(LOCALIZE(i))
#define AS_FUNC(i)   AS_FUNC_PTR(LOCALIZE(i))

void raise(string, string);

int adds(int, int);
int subs(int, int);
int muls(int, int);
int divs(int, int);
int negates(int);


#endif