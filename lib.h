#ifndef LIB_H_INCLUDED
#define LIB_H_INCLUDED

#include "lang.h"

#define MAX_SIZE 2048

//variable_info stores information about a variable :
//the value, the data type and whether or not it's of reference type
typedef struct
{
    long long value;
    int num_of_ptr;
    int is_ref;
} variable_info;

variable_info * init_variable_info();

//func_proc_info stores information about a function or process
//It stores the var_list and the body command
typedef struct
{
    struct var_list * args; 
    struct cmd * body;
}  func_proc_info;

func_proc_info * init_func_proc_info();

//This is the hash table : var_name -> variable_info
struct SLL_hash_table;
unsigned int build_nat(char * c, int len);
char * new_str(char * str, int len);
struct SLL_hash_table * init_SLL_hash();
variable_info * SLL_hash_get(struct SLL_hash_table * t, char * key);
void SLL_hash_set(struct SLL_hash_table * t, char * key, variable_info * value);
void SLL_hash_delete(struct SLL_hash_table * t, char * key);

//This is the hash table : func / proc name -> their info
struct FSLL_hash_table;
struct FSLL_hash_table * Finit_SLL_hash();
func_proc_info * FSLL_hash_get(struct FSLL_hash_table * t, char * key);
void FSLL_hash_set(struct FSLL_hash_table * t, char * key, func_proc_info * value);
void FSLL_hash_delete(struct FSLL_hash_table * t, char * key);


//This is the stack stores the var hash table 
typedef struct {
    struct SLL_hash_table * array[MAX_SIZE];
    int top;
} Stack;


Stack * init_stack();
int isempty(Stack * stack);
int isfull(Stack * stack);
void push(Stack * stack, struct SLL_hash_table * value);
struct SLL_hash_table * pop(Stack * stack);
struct SLL_hash_table * peek(Stack * stack);

#endif
