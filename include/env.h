#ifndef __ENV_HEADER__
#define __ENV_HEADER__

#include <c_types.h>

typedef struct{
    uint16 deepth;
    uint32 offset;
} var;

#define VAR(d, o) ((var){.deepth = d, .offset = o})

extern list   global_sym_map;
extern list   global_var_map;

extern list   sym_map;
extern list   var_map;
extern uint32 var_idx;
extern stack  section_stack;

void ENV_init();

void ENV_push();
void ENV_pop();

int  ENV_set_var(str_view name);
int  ENV_get_var(str_view name);
bool ENV_has_var(str_view name);

#define FIND_VAR(name) ({int v_id = ENV_get_var(name); list_get(var, &var_map, v_id);})

struct __ENV__{
    void (*init)();

    void (*push)();
    void (*pop)();

    int  (*set_var)(str_view);
    int  (*get_var)(str_view);
    bool (*has_var)(str_view);
};
extern struct __ENV__ env;


#endif