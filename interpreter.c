#include <stdlib.h>
#include "lang.h"
#include "interpreter.h"
#include "lib.h"

struct FSLL_hash_table * func_proc_list;
Stack * var_stack;


//I have to have a mark to show that whether the first command in evalution context is 
//sequential or in loop
enum Type 
{
  Seq, WhileBody
};

struct cont 
{
  enum Type type;
  struct cmd * c;
};//the continuation consist of a command and the type of the command

struct cont_list 
{
  struct cont * data;
  struct cont_list * link;
};
//continuation list is a linked list whose node is of type continuation

struct res_prog 
{
  struct cmd * foc;
  struct cont_list * ectx;
};
//residual program consists of two part: focused program and evalution context

struct res_prog * new_res_prog_ptr() 
{
  struct res_prog * res = (struct res_prog *) malloc(sizeof(struct res_prog));
  if (res == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  return res;
}//a function to create a new pointer to residual program 

struct cont * new_cont_ptr() 
{
  struct cont * res = (struct cont *) malloc(sizeof(struct cont));
  if (res == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  return res;
}//a function to create a pointer to the continuation type

struct cont_list * new_cont_list_ptr() 
{
  struct cont_list * res = (struct cont_list *) malloc(sizeof(struct cont_list));
  if (res == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  return res;
}//a function to create a pointer to continuation list

struct cont_list * CL_Nil() 
{
  return NULL;
}//function to create the continuation list: Nil

struct cont_list * CL_Cons(struct cmd * c, struct cont_list * l, enum Type type)
{
  struct cont_list * res = new_cont_list_ptr();
  res -> data = new_cont_ptr();
  res -> data -> c = c;
  res -> data -> type = type;
  res -> link = l;
  return res;
}
//function to create the continuation list: Cons
//modify the cons function to make it compatible with type

struct res_prog * init_res_prog(struct glob_item_list * globlist) 
{
  struct res_prog * res = new_res_prog_ptr();
  struct SLL_hash_table * glob_var_state = init_SLL_hash();
  var_stack = init_stack();
  func_proc_list = Finit_SLL_hash();
  //initializing
  
  struct glob_item_list * p = globlist;
  while(p != NULL)
  {
    switch (p -> data -> t)
    {
    case T_GLOB_VAR:
    {
      variable_info * new_var_info = init_variable_info();
      new_var_info -> value = 0;
      new_var_info -> num_of_ptr = p -> data -> d.GLOB_VAR.num_of_ptr; 
      new_var_info -> is_ref = 0;
      SLL_hash_set(glob_var_state, p -> data -> d.GLOB_VAR.name, new_var_info);
      break;
    }
    case T_FUNC_DEF:
      
      break;
    case T_PROC_DEF:
      
      break;
    default:
      break;
    }
    p = p -> next;
  }

  push(var_stack, glob_var_state);

  //FOR DEBUGING
  struct SLL_hash_table * top = peek(var_stack);
  variable_info * top_var_a = SLL_hash_get(top, "a");
  variable_info * top_var_b = SLL_hash_get(top, "b");
  variable_info * top_var_c = SLL_hash_get(top, "c");
  printf("%lld %d %d \n", top_var_a -> value, top_var_a -> num_of_ptr, top_var_a -> is_ref);
  printf("%lld %d %d \n", top_var_b -> value, top_var_b -> num_of_ptr, top_var_b -> is_ref);
  printf("%lld %d %d \n", top_var_c -> value, top_var_c -> num_of_ptr, top_var_c -> is_ref);
  //

  // res -> foc = c;
  // res -> ectx = CL_Nil();
  
  return res;
}

/*
long long eval(struct expr * e) 
{
  switch (e -> t) {
  case T_CONST:
    return (long long) e -> d.CONST.value;
  case T_VAR:
    return SLL_hash_get(var_state, e -> d.VAR.name);
  case T_BINOP:
    if (e -> d.BINOP.op == T_AND) {
      if (eval(e -> d.BINOP.left)) {
        return eval(e -> d.BINOP.right);
      }
      else {
        return 0;
      }
    }
    else if (e -> d.BINOP.op == T_OR) {
      if (eval(e -> d.BINOP.left)) {
        return 1;
      }
      else {
        return eval(e -> d.BINOP.right);
      }
    }
    else {
      long long left_val = eval(e -> d.BINOP.left);
      long long right_val = eval(e -> d.BINOP.right);
      switch (e -> d.BINOP.op) {
      case T_PLUS:
        return left_val + right_val;
      case T_MINUS:
        return left_val - right_val;
      case T_MUL:
        return left_val * right_val;
      case T_DIV:
        return left_val / right_val;
      case T_MOD:
        return left_val % right_val;
      case T_LT:
        return left_val < right_val;
      case T_GT:
        return left_val > right_val;
      case T_LE:
        return left_val <= right_val;
      case T_GE:
        return left_val >= right_val;
      case T_EQ:
        return left_val == right_val;
      case T_NE:
        return left_val != right_val;
      default:
        return 0; // impossible case
      }
    }
  case T_UNOP:
    if (e -> d.UNOP.op == T_NOT) {
      return ! eval(e -> d.UNOP.arg);
    }
    else {
      return - eval(e -> d.UNOP.arg);
    }
  case T_DEREF:
    return * (long long *) eval(e -> d.DEREF.arg);
  case T_MALLOC: {
    long long arg_val = eval(e -> d.MALLOC.arg);
    if (arg_val % 8 != 0) {
      arg_val = (arg_val | 7) + 1;
    }
    return (long long) malloc(arg_val);
  }
  case T_RI: {
    long long res;
    scanf("%lld", & res);
    return res;
  }
  case T_RC: {
    char res;
    scanf("%c", & res);
    return (long long) res;
  }
  }
}//evaluation function

void step(struct res_prog * r) 
{
  if (r -> foc == NULL) 
  {
      struct cont_list * cl = r -> ectx;
      struct cont * k = cl -> data;
      r -> foc = k -> c;
      r -> ectx = cl -> link;
      free(cl);
  }
  //if the focused program is null we take the first continuation in 
  //cont_list as focused program and let the rest be evaluaton context
  else {
    struct cmd * c = r -> foc;
    switch (c -> t) {
    case T_DECL:
      r -> foc = NULL;
      break;
    case T_ASGN:
      switch (c -> d.ASGN.left -> t) {
      case T_VAR: {
        long long rhs = eval(c -> d.ASGN.right);
        SLL_hash_set(var_state, c -> d.ASGN.left -> d.VAR.name, rhs);
        break;
      }
      case T_DEREF: { 
        long long * lhs = (long long *) eval(c -> d.ASGN.left -> d.DEREF.arg);
        long long rhs = eval(c -> d.ASGN.right);
        * lhs = rhs;
        break;
      }
      default:
        printf("error!\n");
        exit(0);
      }
      r -> foc = NULL;
      break;
    case T_SEQ:
      r -> foc = c -> d.SEQ.left;
      r -> ectx = CL_Cons(c -> d.SEQ.right, r -> ectx, Seq);
      break; 
    case T_IF:
      if (eval(c -> d.IF.cond)) {
        r -> foc = c -> d.IF.left;
      }
      else {
        r -> foc = c -> d.IF.right;
      }
      break;
    case T_WHILE:
      if (eval(c -> d.WHILE.cond)) {
        r -> foc = c -> d.WHILE.body;
        r -> ectx = CL_Cons(c, r -> ectx, WhileBody);
      }
      else {
        r -> foc = NULL;
      }
      break;
    case T_BREAK: 
    {
      struct cont_list * cl = r -> ectx;
      if(cl -> data -> type == Seq)
      {
        r -> ectx = cl -> link;
        free(cl);
      }// If right now the type of evalution context is Seq,
       // we ignore this command and let the rest to be evaluation context
      else
      {
        r -> foc = NULL;
        r -> ectx = cl -> link;
        free(cl);
      }
      // If the type is WhileBody
      // we clear the focused program and let the rest to be evaluation context
      break;
    }
    case T_CONTINUE:
    {
      struct cont_list * cl = r -> ectx;
      if(cl -> data -> type == Seq)
      {
        r -> ectx = cl -> link;
        free(cl);
      }
      // If right now the type of evalution context is Seq,
       // we ignore this command and let the rest to be evaluation context
      else
      {
        r -> foc = cl -> data -> c;
        r -> ectx = cl -> link;
        free(cl);
      }
      break;
      //If the type is WhileBody
      //we take the whilebody as the focused program(cl -> data -> c)
      //Just like what "continue" command do
    }
    case T_WI: {
      long long rhs = eval(c -> d.WI.arg);
      printf("%lld", rhs);
      r -> foc = NULL;
      break;
    }
    case T_WC: {
      char rhs = (char) eval(c -> d.WC.arg);
      printf("%c", rhs);
      r -> foc = NULL;
      break;
    }
    }
  }
}

int test_end(struct res_prog * r) {
  if (r -> foc == NULL && r -> ectx == NULL) {
    return 1;
  }
  else {
    return 0;
  }
}//if the focused program and the evaluation context are all empty then we end the program


*/