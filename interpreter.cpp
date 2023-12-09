#include "lang.h"
#include "interpreter.h"
#include "lib_new.h"
#include <unordered_map>
#include <memory>
#include <list>
#include <string>
#include <iostream>
#include <stack>

//I rewrite this .c file to be .cpp file to make it more readable and 
//easy to debug and convenient to implement

long long variable_info::cnt = 0;

std::unordered_map<long long, long long> loc_val;
//a hash table ampping location and value

std::unordered_map<std::string, func_proc_info> func_proc_table;
std::stack<std::unordered_map<std::string, variable_info>> var_stack;
//a hash table mapping the variable name to the value, the ptr count and the location

//I have to have a mark to show that whether the first command in evalution context is 
//sequential or in loop

enum class Type 
{
  Seq, WhileBody
};

class cont
{
public:
  cont();
  cont(enum Type t, struct cmd * command) : type(t), c(command) {}
  enum Type type;
  struct cmd * c;
};//the continuation consist of a command and the type of the command

class res_prog 
{
public:
  struct cmd * foc;
  std::list< std::shared_ptr<cont> > ectx;
  //continuation list is a linked list whose node is of type continuation
};
//residual program consists of two part: focused program and evalution context


void ConList_push_front(struct cmd * c, std::list< std::shared_ptr<cont> > & cont_l, enum Type type)
{
  std::shared_ptr<cont> new_cont = std::make_shared<cont>(type, c);
  cont_l.push_front(new_cont);
}
//function to create the continuation list: Cons

void ConList_push_back(struct cmd * c, std::list< std::shared_ptr<cont> > & cont_l, enum Type type)
{
  std::shared_ptr<cont> new_cont = std::make_shared<cont>(type, c);
  cont_l.push_back(new_cont);
}

void ConList_pop_back(std::list< std::shared_ptr<cont> > & cont_l)
{
  cont_l.pop_back();
}

void ConList_pop_front(std::list< std::shared_ptr<cont> > & cont_l)
{
  cont_l.pop_front();
}

res_prog * init_res_prog(struct glob_item_list * globlist) 
{
  res_prog * res = new res_prog;
  std::unordered_map<std::string, variable_info> glob_var_state;
  //Now I go through the globlist to make global variable list and func_proc list
  struct glob_item_list * p = globlist;
  while(p != NULL)
  {
    switch (p -> data -> t)
    {
    case T_GLOB_VAR:
    {
      loc_val[variable_info::cnt] = 0;
      variable_info new_var_info(p -> data -> d.GLOB_VAR.num_of_ptr);
      //std::cout << "cnt right now " << variable_info::cnt << std::endl;
      glob_var_state[(std::string)p -> data -> d.GLOB_VAR.name] = new_var_info;
      variable_info::cnt--;
      //std::cout << "cnt right now " << variable_info::cnt << std::endl;
      break;
    }
    case T_FUNC_DEF:
    {
      func_proc_info new_func_info(p -> data -> d.FUNC_DEF.args, p -> data -> d.FUNC_DEF.body, func_proc_info_type::func);
      func_proc_table[(std::string)p -> data -> d.FUNC_DEF.name] = new_func_info;
      break;
    }
    case T_PROC_DEF:
    {
      func_proc_info new_proc_info(p -> data -> d.PROC_DEF.args, p -> data -> d.PROC_DEF.body, func_proc_info_type::proc);
      func_proc_table[(std::string)p -> data -> d.PROC_DEF.name] = new_proc_info;
      break;
    }
    default:
      break;
    }
    p = p -> next;
  }
  var_stack.push(glob_var_state);
  //Now I have a hashtable for all the function and process definition
  //And a stack representing the global variable

  auto entrance = func_proc_table["main"];
  res -> foc = entrance.body;
  //Find the main function and jump into it
  return res;
}


long long eval(struct expr * e) 
{
  switch (e -> t) {
  case T_CONST:
    return (long long) e -> d.CONST.value;
    
  case T_VAR:
    return loc_val[var_stack.top()[e -> d.VAR.name].location];

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
    return loc_val[eval(e -> d.DEREF.arg)];

  case T_ADDR_OF:
  {
    return var_stack.top()[e -> d.ADDR_OF.arg -> d.VAR.name].location;
  }

  case T_MALLOC: {
    long long arg_val = eval(e -> d.MALLOC.arg);
    if (arg_val % 8 != 0) {
      arg_val = (arg_val | 7) + 1;
    }
    return (long long) malloc(arg_val);
  }

  case T_RI: {
    long long res;
    std::cin >> res;
    return res;
  }
  case T_RC: {
    char res;
    std::cin >> res;
    return (long long) res;
  }
  }
  return 1;
}//evaluation function

void step(res_prog * r) 
{
  if (r -> foc == NULL) 
  {
      std::shared_ptr<cont> k = r -> ectx.front();
      r -> ectx.pop_front();
      r -> foc = k -> c;
  }
  //if the focused program is null we take the first continuation in 
  //cont_list as focused program and let the rest be evaluaton context
  else {
    struct cmd * c = r -> foc;
    switch (c -> t) {
    //declaration
    case T_DECL:
    {
        loc_val[variable_info::cnt] = 0;
        variable_info decl(c -> d.DECL.num_of_ptr);
        var_stack.top()[c -> d.DECL.name] = decl;
        variable_info::cnt--;
        r -> foc = c -> d.DECL.body;
        break;
    }
    case T_ASGN:
      switch (c -> d.ASGN.left -> t) {
      case T_VAR: {
        long long rhs = eval(c -> d.ASGN.right);
        loc_val[var_stack.top()[c -> d.ASGN.left -> d.VAR.name].location] = rhs;
        r -> foc = NULL;
        break;
      }
    case T_DEREF: { 
        long long lhs = eval(c -> d.ASGN.left -> d.DEREF.arg);
        long long rhs = eval(c -> d.ASGN.right);
        loc_val[lhs] = rhs;
        r -> foc = NULL;
        break;
      }
      default:
        printf("error!\n");
        exit(0);
      }
      break;

    case T_DECL_ASGN:
    {
      variable_info decl(c -> d.DECL_ASGN.num_of_ptr);
      var_stack.top()[c -> d.DECL_ASGN.name] = decl;
      variable_info::cnt--;
      long long rhs = eval(c -> d.DECL_ASGN.right);
      loc_val[var_stack.top()[c -> d.DECL_ASGN.name].location] = rhs;
      r -> foc = c -> d.DECL_ASGN.body;
      break;
    }

    case T_REF_DECL_ASGN:
    {
      
      break;
    }

    case T_SEQ:
      r -> foc = c -> d.SEQ.left;
      ConList_push_front(c -> d.SEQ.right, r -> ectx, Type::Seq);
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
        ConList_push_front(c, r -> ectx, Type::WhileBody);
      }
      else {
        r -> foc = NULL;
      }
      break;

    case T_BREAK: 
    {
      if(r -> ectx.front() -> type == Type::Seq)
      {
        r -> ectx.pop_front();
      }//right now the type of evalution context is Seq
      else
      {
        r -> foc = NULL;
        r -> ectx.pop_front();
      }
      // If the type is WhileBody
      // we clear the focused program and let the rest to be evaluation context
      break;
    }

    case T_CONTINUE:
    {
      if(r -> ectx.front() -> type == Type::Seq)
      {
        r -> ectx.pop_front();
      }
      //right now the type of evalution context is Seq
    
      else
      {
        r -> foc = r -> ectx.front() -> c;
        r -> ectx.pop_front();
      }
      break;
      //If the type is WhileBody
      //we take the whilebody as the focused program(cl.front() -> c)
      //Just like what "continue" command do
    }
    case T_WI: {
      long long rhs = eval(c -> d.WI.arg);
      std::cout << rhs;
      r -> foc = NULL;
      break;
    }
    case T_WC: {
      char rhs = (char) eval(c -> d.WC.arg);
      std::cout << rhs;
      r -> foc = NULL;
      break;
    }
    }
  }
}

int test_end(res_prog * r) {
  if (r -> foc == NULL && r -> ectx.empty()) 
  {
    //std::cout << 1 << std::endl;
    return 1;
  }
  else {
    //std::cout << 0 << std::endl;
    // if(var_stack.top().count("n") == 1)
    // {
    //   std::cout << "the location for n is " << var_stack.top()["n"].location << std::endl;
    // }
    // if(var_stack.top().count("m") == 1)
    // {
    //   std::cout << "the location for m is " << var_stack.top()["m"].location << std::endl;
    // }
    // if(var_stack.top().count("s") == 1)
    // {
    //   std::cout << "the location for s is " << var_stack.top()["s"].location << std::endl;
    // }
    return 0;
  }
}//if the focused program and the evaluation context are all empty then we end the program
