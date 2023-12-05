#include <stdlib.h>
#include "lang.h"
#include "interpreter.h"
#include <unordered_map>
#include <memory>
#include <list>
#include <string>


//I rewrite this .c file to be .cpp file to make it more readable and 
//easy to debug and convenient to implement


std::unordered_map<std::string, long long> var_state;
//a hash table mapping the variable name to the value and the ptr count

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

res_prog * init_res_prog(struct cmd * c) 
{
  res_prog * res = new res_prog;
  res -> foc = c;
  return res;
}//a function to initialize the residual program

long long eval(struct expr * e) 
{
  switch (e -> t) {
  case T_CONST:
    return (long long) e -> d.CONST.value;

    
  case T_VAR:
    return var_state[e -> d.VAR.name];


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
  return 1;
}//evaluation function

void step(res_prog * r) 
{
  if (r -> foc == NULL) 
  {
      std::list< std::shared_ptr<cont> > cl = r -> ectx;
      std::shared_ptr<cont> k = cl.front();
      cl.pop_front();
      r -> foc = k -> c;
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
        var_state[c -> d.ASGN.left -> d.VAR.name] = rhs;
        
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
      std::list<std::shared_ptr<cont> > cl = r -> ectx;
      if(cl.front() -> type == Type::Seq)
      {
        cl.pop_front();
      }//right now the type of evalution context is Seq
      else
      {
        r -> foc = NULL;
        cl.pop_front();
      }
      // If the type is WhileBody
      // we clear the focused program and let the rest to be evaluation context
      break;
    }

    case T_CONTINUE:
    {
      std::list<std::shared_ptr<cont> > cl = r -> ectx;
      if(cl.front() -> type == Type::Seq)
      {
        cl.pop_front();
      }
      //right now the type of evalution context is Seq
    
      else
      {
        r -> foc = cl.front() -> c;
        cl.pop_front();
      }
      break;
      //If the type is WhileBody
      //we take the whilebody as the focused program(cl.front() -> c)
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
  if (r -> foc == NULL && r -> ectx.empty()) {
    return 1;
  }
  else {
    return 0;
  }
}//if the focused program and the evaluation context are all empty then we end the program