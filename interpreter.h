#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED

#include "lang.h"

struct res_prog;
struct res_prog * init_res_prog(struct glob_item_list * g);
void step(struct res_prog * r);
int test_end(struct res_prog * r);

#endif
