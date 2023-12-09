#ifndef LIB_NEWH_INCLUDED
#define LIB_NEWH_INCLUDED

#include "lang.h"

#define MAX_SIZE 2048

enum class func_proc_info_type
{
    func,
    proc,
};

class variable_info
{
public:
    variable_info()
    {
        location = cnt;
        cnt++;
    }
    variable_info(int num2) : num_of_ptr(num2), is_ref(0)
    {
        location = cnt;
        cnt++;
    }
    variable_info(int num2, int num3) : num_of_ptr(num2), is_ref(num3)
    {
        location = cnt;
        cnt++;
    }
    variable_info(const variable_info & other)
    {
        if(this == &other)
        {
            return;
        }
        num_of_ptr = other.num_of_ptr;
        is_ref = other.is_ref;
        location = other.location;
    }
    int num_of_ptr;
    int is_ref;
    long long location;

    static long long cnt;
};

class func_proc_info
{
public:
    func_proc_info(){};
    func_proc_info(struct var_list * v, struct cmd * b, func_proc_info_type t) : args(v), body(b), type(t){}
    struct var_list * args; 
    struct cmd * body;
    func_proc_info_type type;
};


struct SLL_hash_table;
unsigned int build_nat(char * c, int len);
char * new_str(char * str, int len);
struct SLL_hash_table * init_SLL_hash();
int SLL_hash_get(struct SLL_hash_table * t, char * key);
void SLL_hash_set(struct SLL_hash_table * t, char * key, int value);
void SLL_hash_delete(struct SLL_hash_table * t, char * key);

#endif