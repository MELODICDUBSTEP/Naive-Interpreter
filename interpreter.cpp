#include "lang.h"
#include "interpreter.h"
#include "lib_new.h"
#include <unordered_map>
#include <memory>
#include <list>
#include <string>
#include <iostream>
#include <vector>

//I rewrite this .c file to be .cpp file to make it more readable and 
//easy to debug and convenient to implement

long long variable_info::cnt = 0;

std::unordered_map<long long, long long> loc_val;
//a hash table ampping location and value

std::unordered_map<std::string, func_proc_info> func_proc_table;
std::vector<std::unordered_map<std::string, variable_info>> var_vector;
//a hash table mapping the variable name to the value, the ptr count and the location
std::vector<std::vector<std::unordered_map<std::string, variable_info>>> var_vector_temp;
//var_vector_temp will store the var_vector when calling process or function

variable_info & search_var(std::string name)
{
  for(int i = var_vector.size() - 1; i >= 0; i--)
  {
    if(var_vector[i].count(name) == 1)
    {
      return var_vector[i][name];
    }
  }
  std::cout << "There's no variable named " << name << std::endl;
  return var_vector[0][name];
}

variable_info & Fsearch_var(std::string name)
{
  if(var_vector[var_vector.size() - 1].count(name) == 1)
    {
      return var_vector[var_vector.size() - 1][name];
    }
  return var_vector[0][name];
}

//I have to have a mark to show that whether the first command in evalution context is 
//sequential or in loop

enum class Type 
{
  Seq, 
  WhileBody,
  BinopL,
  BinopR
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
      glob_var_state[(std::string)p -> data -> d.GLOB_VAR.name] = new_var_info;
      variable_info::cnt--;
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
  var_vector.push_back(glob_var_state);
  //Now I have a hashtable for all the function and process definition
  //And a stack representing the global variable

  auto entrance = func_proc_table["main"];
  std::unordered_map<std::string, variable_info> new_map;
  var_vector.push_back(new_map);
  res -> foc = entrance.body;
  //Find the main function and jump into it
  return res;
}


long long eval(struct expr * e, res_prog * r) 
{
  switch (e -> t) {
  case T_CONST:
    return (long long) e -> d.CONST.value;
    
  case T_VAR:
    return loc_val[search_var(e -> d.VAR.name).location];

  case T_FUNC:
    {
      std::unordered_map<std::string, variable_info> new_map;
      func_proc_info info = func_proc_table[e -> d.FUNC.name];
      //assign the temporary variables
      struct var_list * var_l = info.args;
      struct expr_list * expr_l = e -> d.FUNC.args;
      var_vector.push_back(new_map);

      //I have to have a variable for me to store the return value
      std::string ret = "__return";
      loc_val[variable_info::cnt] = 0;
      variable_info decl;
      var_vector[var_vector.size() - 1][ret] = decl;
      variable_info::cnt--;
      //preprocessing : create these variables in function definition
      


      while(var_l != NULL)
      {
        if(var_l -> is_ref == 0)
        {
          //similar to DECL_ASGN
            variable_info decl(var_l -> num_of_ptr);
            long long rhs = eval(expr_l -> data, r);
            var_vector[var_vector.size() - 1][var_l -> name] = decl;
            variable_info::cnt--;
            loc_val[search_var(var_l -> name).location] = rhs;
        }
        else
        {
          //similar to REF_DECL_ASGN
            variable_info decl(var_l -> num_of_ptr);
            decl.location = search_var(expr_l -> data -> d.VAR.name).location;
            var_vector[var_vector.size() - 1][var_l -> name] = decl;
            variable_info::cnt--;
        }
        var_l = var_l -> next;
        expr_l = expr_l -> next;
      }

      std::vector<std::unordered_map<std::string, variable_info>> tmp_vector = var_vector;
      tmp_vector.pop_back();
      std::unordered_map<std::string, variable_info> tmp_map1 = var_vector[0];
      std::unordered_map<std::string, variable_info> tmp_map2 = var_vector.back();
      var_vector.clear();
      var_vector.push_back(tmp_map1);
      var_vector.push_back(tmp_map2);
      var_vector_temp.push_back(tmp_vector);

      //A special way to implement function calling:
      //create a new set of foc and ectx
      res_prog res_in_func;
      res_in_func.foc = info.body;
      struct cmd * end = new_cmd_ptr();
      end -> t = T_END_FUNC;
      //This command mark the end of function calling
      ConList_push_front(end, res_in_func.ectx, Type::Seq); 

      while( !test_end(&res_in_func))
      {
        step(&res_in_func);
      }

      //return the value
      long long value_ret = loc_val[var_vector[var_vector.size() - 1][ret].location];
      std::vector<std::unordered_map<std::string, variable_info>> tmp = var_vector_temp.back();
      var_vector_temp.pop_back();
      var_vector = tmp;
      //erase __return in the hash table
      return value_ret;
    }

  case T_BINOP:
    if (e -> d.BINOP.op == T_AND) {
      if (eval(e -> d.BINOP.left, r)) {
        return eval(e -> d.BINOP.right, r);
      }
      else {
        return 0;
      }
    }
    else if (e -> d.BINOP.op == T_OR) {
      if (eval(e -> d.BINOP.left, r)) {
        return 1;
      }
      else {
        return eval(e -> d.BINOP.right, r);
      }
    }
    else {
      long long left_val = eval(e -> d.BINOP.left, r);
      long long right_val = eval(e -> d.BINOP.right, r);
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
      return ! eval(e -> d.UNOP.arg, r);
    }
    else {
      return - eval(e -> d.UNOP.arg, r);
    }

  case T_DEREF:
    return loc_val[eval(e -> d.DEREF.arg, r)];

  case T_ADDR_OF:
  {
    return search_var(e -> d.ADDR_OF.arg -> d.VAR.name).location;
  }

  case T_MALLOC: {
    long long arg_val = eval(e -> d.MALLOC.arg, r);
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

//The return type for step is to decide when should I go out of
//stepping
bool step(res_prog * r) 
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
        var_vector[var_vector.size() - 1][c -> d.DECL.name] = decl;
        variable_info::cnt--;
        r -> foc = NULL;
        break;
    }

    //{ C }
    case T_LOCAL:
    {
        //create a new hash map and delete it when going out of the brackets
        std::unordered_map<std::string, variable_info> new_map;
        var_vector.push_back(new_map);
        r -> foc = c -> d.LOCAL.body;
        struct cmd * out_of_scope = new_cmd_ptr();
        out_of_scope -> t = T_OUT;
        ConList_push_front(out_of_scope, r -> ectx, Type::Seq);
        break;
    }

    //a label to design the local command : { C }
    case T_OUT:
    { 
        var_vector.pop_back();
        r -> foc = NULL;
        break;
    }

    case T_ASGN:
      switch (c -> d.ASGN.left -> t) {
      case T_VAR: {
        long long rhs = eval(c -> d.ASGN.right, r);
        loc_val[search_var(c -> d.ASGN.left -> d.VAR.name).location] = rhs;
        r -> foc = NULL;
        break;
      }

    case T_DEREF: { 
        long long lhs = eval(c -> d.ASGN.left -> d.DEREF.arg, r);
        long long rhs = eval(c -> d.ASGN.right, r);
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
      var_vector[var_vector.size() - 1][c -> d.DECL_ASGN.name] = decl;
      variable_info::cnt--;
      long long rhs = eval(c -> d.DECL_ASGN.right, r);
      loc_val[search_var(c -> d.DECL_ASGN.name).location] = rhs;
      r -> foc = NULL;
      break;
    }

    case T_REF_DECL_ASGN:
    {
      variable_info decl(c -> d.REF_DECL_ASGN.num_of_ptr);
      decl.location = search_var(c -> d.REF_DECL_ASGN.right).location;
      var_vector[var_vector.size() - 1][c -> d.REF_DECL_ASGN.name] = decl;
      variable_info::cnt--;
      r -> foc = NULL;
      break;
    }

    case T_SEQ:
      r -> foc = c -> d.SEQ.left;
      ConList_push_front(c -> d.SEQ.right, r -> ectx, Type::Seq);
      break; 

    case T_END_FUNC:
    {
      r -> foc = NULL;
      return 1;
    }

    case T_END_PROC:
    {
      std::vector<std::unordered_map<std::string, variable_info>> tmp = var_vector_temp.back();
      var_vector_temp.pop_back();
      var_vector = tmp;

      r -> foc = NULL;
      break;
    }

    case T_PROC:
    {
      std::unordered_map<std::string, variable_info> new_map;
      func_proc_info info = func_proc_table[c -> d.PROC.name];
      var_vector.push_back(new_map);
      //assign the temporary variables
      struct var_list * var_l = info.args;
      struct expr_list * expr_l = c -> d.PROC.args;

      while(var_l != NULL)
      {
        if(var_l -> is_ref == 0)
        {
            long long rhs = eval(expr_l -> data, r);
            variable_info decl(var_l -> num_of_ptr);
            var_vector[var_vector.size() - 1][var_l -> name] = decl;
            variable_info::cnt--;
            loc_val[search_var(var_l -> name).location] = rhs;
        }
        else
        {
            variable_info decl(var_l -> num_of_ptr);
            decl.location = search_var(expr_l -> data -> d.VAR.name).location;
            var_vector[var_vector.size() - 1][var_l -> name] = decl;
            variable_info::cnt--;
        }
        var_l = var_l -> next;
        expr_l = expr_l -> next;
      }

      std::vector<std::unordered_map<std::string, variable_info>> tmp_vector = var_vector;
      tmp_vector.pop_back();
      std::unordered_map<std::string, variable_info> tmp_map1 = var_vector[0];
      std::unordered_map<std::string, variable_info> tmp_map2 = var_vector.back();
      var_vector.clear();
      var_vector.push_back(tmp_map1);
      var_vector.push_back(tmp_map2);
      var_vector_temp.push_back(tmp_vector);

      r -> foc = info.body;

      struct cmd * end = new_cmd_ptr();
      end -> t = T_END_PROC;
      ConList_push_front(end, r -> ectx, Type::Seq);  
      //I add a end command to the evaluation context
      //so that I can delete the variable info stack 
      //when I leave a process

      break;
    }

    case T_IF:
      if (eval(c -> d.IF.cond, r)) {
        r -> foc = c -> d.IF.left;
      }
      else {
        r -> foc = c -> d.IF.right;
      }
      break;

    case T_FOR:
      {
        r -> foc = c -> d.FOR.init;
        struct cmd * while_cmd = new_cmd_ptr();
        while_cmd -> t = T_WHILE;
        while_cmd -> d.WHILE.cond = c -> d.FOR.cond;
        struct cmd * while_body = new_cmd_ptr();
        while_body -> t = T_SEQ;
        while_body -> d.SEQ.left = c -> d.FOR.body;
        while_body -> d.SEQ.right = c -> d.FOR.incr;
        while_cmd -> d.WHILE.body = while_body;
        ConList_push_front(while_cmd, r -> ectx, Type::Seq);
        break;
      }

    case T_WHILE:
    {
      long long result = eval(c -> d.WHILE.cond, r);
      std::unordered_map<std::string, variable_info> new_map;
      var_vector.push_back(new_map);
      r -> foc = c -> d.LOCAL.body;
      struct cmd * out_of_scope = new_cmd_ptr();
      out_of_scope -> t = T_OUT;
      ConList_push_front(out_of_scope, r -> ectx, Type::Seq);
      if (result) {
        r -> foc = c -> d.WHILE.body;
        ConList_push_front(c, r -> ectx, Type::WhileBody);
      }
      else {
        r -> foc = NULL;
      }
      break;
    }
    case T_DO_WHILE:
    {
      cmd * while_cmd = new_cmd_ptr();
      while_cmd -> t = T_WHILE;
      while_cmd -> d.WHILE.body = c -> d.DO_WHILE.body;
      while_cmd -> d.WHILE.cond = c -> d.DO_WHILE.cond;
      ConList_push_front(while_cmd, r -> ectx, Type::Seq);
      ConList_push_front(c -> d.DO_WHILE.body, r -> ectx, Type::Seq);
      r -> foc = NULL;
      break;
    }

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
      long long rhs = eval(c -> d.WI.arg, r);
      std::cout << rhs;
      r -> foc = NULL;
      break;
    }
    case T_WC: {
      char rhs = (char) eval(c -> d.WC.arg, r);
      std::cout << rhs;
      r -> foc = NULL;
      break;
    }
    }
  }
  return 0;
}

int test_end(res_prog * r) {
  if (r -> foc == NULL && r -> ectx.empty()) 
  {
    return 1;
  }
  else {
    return 0;
  }
}//if the focused program and the evaluation context are all empty then we end the program
