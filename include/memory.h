#ifndef __MEMORY_HEADER__
#define __MEMORY_HEADER__

#include <c_types.h>

extern list mem;

#define RUNTIME_MEMORY 0
#define STATIC_MEMORY  1

int MEMORY_save_const (int deepth, uint8  value);
int MEMORY_save_number(int deepth, double value);
int MEMORY_save_string(int deepth, str   value);

void* MEMORY_get(int deepth, int index);
#define LOCALIZE(v) ({var _v = v; MEMORY_get(_v.deepth, _v.offset);})

#define CONST_NIL   VAR(STATIC_MEMORY, 0)
#define CONST_FALSE VAR(STATIC_MEMORY, 1)
#define CONST_TRUE  VAR(STATIC_MEMORY, 2)

#define SAVED_NUMBER(deepth, value) (VAR(deepth, MEMORY_save_number(deepth, value)))
#define SAVED_STRING(deepth, value) (VAR(deepth, MEMORY_save_string(deepth, value)))


#define GET_TYPE(ptr)   (*((uint8*)ptr) & 0b00011110)

enum VAL_TYPE{
    VAL_NIL      = 0b00000010,
    VAL_FALSE    = 0b00000100,
    VAL_TRUE     = 0b00000110,
    VAL_NUMBER   = 0b00001000,
    VAL_STRING   = 0b00001010,
    VAL_POINTER  = 0b00001100,
    VAL_FUNCTION = 0b00001110,
    VAL_CLASS    = 0b00010000,
    VAL_METHOD   = 0b00010010,
    VAL_CODETXT  = 0b00010100,
    VAL_CODESRC  = 0b00010110,
};

str VAL_FORMAT(void*);

#define MARK_AS_GARBAGE(ptr) (*((uint8*)ptr) |= 0b00000001)
#define IS_GARBAGE(ptr)      (*((uint8*)ptr) & 0b00000001)

#define AS_NUMBER(ptr) (*((double*)(ptr + 1)))
#define AS_STRING(ptr) ((str)(ptr + 1))

void MEMORY_init(void);

struct __MEMORY__ {
    void (*init)(void);

    int (*save_const) (int deepth, uint8  value);
    int (*save_number)(int deepth, double value);
    int (*save_string)(int deepth, str   value);

    void* (*get)(int deepth, int index);
};
extern struct __MEMORY__ memory;

#endif