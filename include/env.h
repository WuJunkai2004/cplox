#ifndef __ENV_HEADER__
#define __ENV_HEADER__

#include <c_types.h>

#include "var.h"

extern list   var_map;
extern uint32 var_idx;
extern stack  section_stack;

void ENV_init();

void ENV_push();
void ENV_pop();

int  ENV_set_var(str_view name, uint16 deepth, uint32 offset);
var  ENV_get_var(str_view name);
bool ENV_has_var(str_view name);

struct __ENV__{
    void (*init)();

    void (*push)();
    void (*pop)();

    int  (*set_var)(str_view, uint16, uint32);
    var  (*get_var)(str_view);
    bool (*has_var)(str_view);
};
extern struct __ENV__ env;


#endif