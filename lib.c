#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"
#include "lang.h"

#define MAX_SIZE 2048

variable_info * init_variable_info()
{
  return (variable_info *)malloc(sizeof(variable_info));
}

func_proc_info * init_func_proc_info()
{
  return (func_proc_info *)malloc(sizeof(func_proc_info));
}

unsigned int build_nat(char * c, int len) {
  int s = 0, i = 0;
  for (i = 0; i < len; ++i) {
    if (s > 429496729) {
      printf("We cannot handle natural numbers greater than 4294967295.\n");
      exit(0);
    }
    if (s == 429496729 && c[i] > '5') {
      printf("We cannot handle natural numbers greater than 4294967295.\n");
      exit(0);
    }
    s = s * 10 + (c[i] - '0');
  }
  return s;
}

char * new_str(char * str, int len) {
  char * res = (char *) malloc(sizeof(char) * (len + 1));
  if (res == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  strcpy(res, str);
  return res;
}

struct SLL_hash_cell {
  char * key;
  variable_info * value;
  struct SLL_hash_cell * tail;
};
  
struct SLL_hash_table{
  struct SLL_hash_cell * (h[MAX_SIZE]);
};

unsigned int hash_fun(char * str) {
  unsigned int s = 0;
  while (str[0] != 0) {
    s = (s * 307 + 97 + str[0]) & 2047;
    str ++;
  }
  return s;
}

struct SLL_hash_table * init_SLL_hash() {
  struct SLL_hash_table * res = (struct SLL_hash_table *) malloc(sizeof(struct SLL_hash_table));
  if (res == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  memset(res, 0, sizeof(struct SLL_hash_table));
  return res;
}

variable_info * SLL_hash_get(struct SLL_hash_table * t, char * key) {
  unsigned int s = hash_fun(key);
  struct SLL_hash_cell * p = t -> h[s];
  while (p != NULL) {
    if (strcmp(key, p -> key) == 0) {
      return p -> value;
    }
    p = p -> tail;
  }
  return NULL;
}

void SLL_hash_set(struct SLL_hash_table * t, char * key, variable_info * value) {
  unsigned int s = hash_fun(key);
  struct SLL_hash_cell * * d = & (t -> h[s]);
  while ((* d) != NULL) {
    if (strcmp(key, (* d) -> key) == 0) {
      (* d) -> value = value;
      return;
    }
    d = & ((* d) -> tail);
  }
  * d = (struct SLL_hash_cell *) malloc(sizeof (struct SLL_hash_cell));
  if (* d == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  (* d) -> key = new_str(key, strlen(key));
  (* d) -> value = value;
  (* d) -> tail = NULL;
  return;
}

void SLL_hash_delete(struct SLL_hash_table * t, char * key) {
  unsigned int s = hash_fun(key);
  struct SLL_hash_cell * * d = & (t -> h[s]);
  while ((* d) != NULL) {
    if (strcmp(key, (* d) -> key) == 0) {
      struct SLL_hash_cell * p = * d;
      * d = p -> tail;
      free(p);
      return;
    }
    d = & ((* d) -> tail);
  }
}


//Below is the hashtable for function or process

struct FSLL_hash_cell {
  char * key;
  func_proc_info * value;
  struct FSLL_hash_cell * tail;
};
  
struct FSLL_hash_table{
  struct FSLL_hash_cell * (h[MAX_SIZE]);
};


struct FSLL_hash_table * Finit_SLL_hash() {
  struct FSLL_hash_table * res = (struct FSLL_hash_table *) malloc(sizeof(struct FSLL_hash_table));
  if (res == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  memset(res, 0, sizeof(struct FSLL_hash_table));
  return res;
}

func_proc_info * FSLL_hash_get(struct FSLL_hash_table * t, char * key) {
  unsigned int s = hash_fun(key);
  struct FSLL_hash_cell * p = t -> h[s];
  while (p != NULL) {
    if (strcmp(key, p -> key) == 0) {
      return p -> value;
    }
    p = p -> tail;
  }
  return NULL;
}

void FSLL_hash_set(struct FSLL_hash_table * t, char * key, func_proc_info * value) {
  unsigned int s = hash_fun(key);
  struct FSLL_hash_cell * * d = & (t -> h[s]);
  while ((* d) != NULL) {
    if (strcmp(key, (* d) -> key) == 0) {
      (* d) -> value = value;
      return;
    }
    d = & ((* d) -> tail);
  }
  * d = (struct FSLL_hash_cell *) malloc(sizeof (struct FSLL_hash_cell));
  if (* d == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  (* d) -> key = new_str(key, strlen(key));
  (* d) -> value = value;
  (* d) -> tail = NULL;
  return;
}

void FSLL_hash_delete(struct FSLL_hash_table * t, char * key) {
  unsigned int s = hash_fun(key);
  struct FSLL_hash_cell * * d = & (t -> h[s]);
  while ((* d) != NULL) {
    if (strcmp(key, (* d) -> key) == 0) {
      struct FSLL_hash_cell * p = * d;
      * d = p -> tail;
      free(p);
      return;
    }
    d = & ((* d) -> tail);
  }
}





// Function to initialize the stack
Stack * init_stack() {
    Stack * stack = (Stack *)malloc(sizeof(Stack));
    stack -> top = -1;
    return stack;
}

// Function to check if the stack is empty
int isempty(Stack *stack) {
    return stack->top == -1;
}

// Function to check if the stack is full
int isfull(Stack * stack) {
    return stack -> top == MAX_SIZE - 1;
}

// Function to push an element onto the stack
void push(Stack *stack, struct SLL_hash_table * value) {
    if (isfull(stack)) {
        printf("Stack overflow! Cannot push element\n");
        return;
    }
    stack->array[++stack -> top] = value;
}

// Function to pop an element from the stack
struct SLL_hash_table * pop(Stack *stack) {
    if (isempty(stack)) {
        printf("Stack underflow! Cannot pop from an empty stack.\n");
        return NULL; // Return a special value to indicate an error
    }
    return stack->array[stack->top--];
}

struct SLL_hash_table * peek(Stack *stack) {
    if (isempty(stack)) {
        printf("Stack is empty! Cannot peek.\n");
        return NULL; // Return a special value to indicate an error
    }
    return stack->array[stack->top];
}
  
//FOR DEBUGING

// int main()
// {
//   variable_info a1 = {0, 0, 1};
//   variable_info * a = &a1;
//   struct SLL_hash_table * my_hash_table = init_SLL_hash();
//   SLL_hash_set(my_hash_table, "a", a);
//   variable_info * b = SLL_hash_get(my_hash_table, "a");
//   SLL_hash_delete(my_hash_table, "a");
// }