#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lang.h"

//This is what I added: I want to have a structure to know that 

struct expr * new_expr_ptr() {
  struct expr * res = (struct expr *) malloc(sizeof(struct expr));
  if (res == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  return res;
}

struct expr_list * new_expr_list_ptr() {
  struct expr_list * res =
    (struct expr_list *) malloc(sizeof(struct expr_list));
  if (res == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  return res;
}

struct cmd * new_cmd_ptr() {
  struct cmd * res = (struct cmd *) malloc(sizeof(struct cmd));
  if (res == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  return res;
}

struct var_list * new_var_list_ptr() {
  struct var_list * res = (struct var_list *) malloc(sizeof(struct var_list));
  if (res == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  return res;
}

struct glob_item * new_glob_item_ptr() {
  struct glob_item * res =
    (struct glob_item *) malloc(sizeof(struct glob_item));
  if (res == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  return res;
}

struct glob_item_list * new_glob_item_list_ptr() {
  struct glob_item_list * res =
    (struct glob_item_list *) malloc(sizeof(struct glob_item_list));
  if (res == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  return res;
}

struct expr_list * TENil() {
  return NULL;
}

struct expr_list * TECons(struct expr * data, struct expr_list * next) {
  struct expr_list * res = new_expr_list_ptr();
  res -> data = data;
  res -> next = next;
  return res;
}

struct expr * TConst(unsigned int value) {
  struct expr * res = new_expr_ptr();
  res -> t = T_CONST;
  res -> d.CONST.value = value;
  return res;
}

struct expr * TVar(char * name) {
  struct expr * res = new_expr_ptr();
  res -> t = T_VAR;
  res -> d.VAR.name = name;
  return res;
}

struct expr * TBinOp(enum BinOpType op, struct expr * left, struct expr * right) {
  struct expr * res = new_expr_ptr();
  res -> t = T_BINOP;
  res -> d.BINOP.op = op;
  res -> d.BINOP.left = left;
  res -> d.BINOP.right = right;
  return res;
}

struct expr * TUnOp(enum UnOpType op, struct expr * arg) {
  struct expr * res = new_expr_ptr();
  res -> t = T_UNOP;
  res -> d.UNOP.op = op;
  res -> d.UNOP.arg = arg;
  return res;
}

struct expr * TDeref(struct expr * arg) {
  struct expr * res = new_expr_ptr();
  res -> t = T_DEREF;
  res -> d.DEREF.arg = arg;
  return res;
}

struct expr * TAddrOf(struct expr * arg) {
  struct expr * res = new_expr_ptr();
  res -> t = T_ADDR_OF;
  res -> d.ADDR_OF.arg = arg;
  return res;
}

struct expr * TMalloc(struct expr * arg) {
  struct expr * res = new_expr_ptr();
  res -> t = T_MALLOC;
  res -> d.MALLOC.arg = arg;
  return res;
}

struct expr * TReadInt() {
  struct expr * res = new_expr_ptr();
  res -> t = T_RI;
  return res;
}

struct expr * TReadChar() {
  struct expr * res = new_expr_ptr();
  res -> t = T_RC;
  return res;
}

struct cmd * TWriteInt(struct expr * arg) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_WI;
  res -> d.WI.arg = arg;
  return res;
}

struct cmd * TWriteChar(struct expr * arg) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_WC;
  res -> d.WC.arg = arg;
  return res;
}

struct expr * TFunc(char * name, struct expr_list * args) {
  struct expr * res = new_expr_ptr();
  res -> t = T_FUNC;
  res -> d.FUNC.name = name;
  res -> d.FUNC.args = args;
  return res;
}

struct cmd* TLocal(struct cmd * body)
{
  struct cmd * res = new_cmd_ptr();
  res -> t = T_LOCAL;
  res -> d.LOCAL.body = body;
  return res;
}

struct cmd * TDecl(char * name, int num_of_ptr) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_DECL;
  res -> d.DECL.name = name;
  res -> d.DECL.num_of_ptr = num_of_ptr;
  return res;
}


struct cmd * TDeclAsgn(char * name, int num_of_ptr, struct expr * right) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_DECL_ASGN;
  res -> d.DECL_ASGN.name = name;
  res -> d.DECL_ASGN.num_of_ptr = num_of_ptr;
  res -> d.DECL_ASGN.right = right;
  return res;
}

struct cmd * TRefDeclAsgn(char * name, int num_of_ptr, char * right)
{
  struct cmd * res = new_cmd_ptr();
  res -> t = T_REF_DECL_ASGN;
  res -> d.REF_DECL_ASGN.name = name;
  res -> d.REF_DECL_ASGN.num_of_ptr = num_of_ptr;
  res -> d.REF_DECL_ASGN.right = right;
  return res;
}

struct cmd * TAsgn(struct expr * left, struct expr * right) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_ASGN;
  res -> d.ASGN.left = left;
  res -> d.ASGN.right = right;
  return res;
}

struct cmd * TSeq(struct cmd * left, struct cmd * right) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_SEQ;
  res -> d.SEQ.left = left;
  res -> d.SEQ.right = right;
  return res;
}

struct cmd * TIf(struct expr * cond, struct cmd * left, struct cmd * right) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_IF;
  res -> d.IF.cond = cond;
  res -> d.IF.left = left;
  res -> d.IF.right = right;
  return res;
}

struct cmd * TWhile(struct expr * cond, struct cmd * body) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_WHILE;
  res -> d.WHILE.cond = cond;
  res -> d.WHILE.body = body;
  return res;
}

struct cmd * TFor(struct cmd * init, struct expr * cond,
                  struct cmd * body, struct cmd * incr) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_FOR;
  res -> d.FOR.init = init;
  res -> d.FOR.cond = cond;
  res -> d.FOR.body = body;
  res -> d.FOR.incr = incr;
  return res;
}

struct cmd * TDoWhile(struct cmd * body, struct expr * cond) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_DO_WHILE;
  res -> d.DO_WHILE.body = body;
  res -> d.DO_WHILE.cond = cond;
  return res;
}

struct cmd * TProc(char * name, struct expr_list * args) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_PROC;
  res -> d.PROC.name = name;
  res -> d.PROC.args = args;
  return res;
}

struct cmd * TBreak() {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_BREAK;
  return res;
}

struct cmd * TContinue() {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_CONTINUE;
  return res;
}

struct var_list * TVNil() {
  return NULL;
}

struct var_list * TVCons(char * name, struct var_list * next, int num_of_ptr, int is_ref) {
  struct var_list * res = new_var_list_ptr();
  res -> name = name;
  res -> num_of_ptr = num_of_ptr;
  res -> is_ref = is_ref;
  res -> next = next;
  return res;
}

struct glob_item * TFuncDef(char * name, struct var_list * args,
                            struct cmd * body) {
  struct glob_item * res = new_glob_item_ptr();
  res -> t = T_FUNC_DEF;
  res -> d.FUNC_DEF.name = name;
  res -> d.FUNC_DEF.args = args;
  res -> d.FUNC_DEF.body = body;
  return res;
}

struct glob_item * TProcDef(char * name, struct var_list * args,
                            struct cmd * body) {
  struct glob_item * res = new_glob_item_ptr();
  res -> t = T_PROC_DEF;
  res -> d.PROC_DEF.name = name;
  res -> d.PROC_DEF.args = args;
  res -> d.PROC_DEF.body = body;
  return res;
}

struct glob_item * TGlobVar(char * name, int num_of_ptr) {
  struct glob_item * res = new_glob_item_ptr();
  res -> t = T_GLOB_VAR;
  res -> d.GLOB_VAR.name = name;
  res -> d.GLOB_VAR.num_of_ptr = num_of_ptr;
  return res;
}

struct glob_item_list * TGNil() {
  return NULL;
}

struct glob_item_list * TGCons(struct glob_item * data,
                               struct glob_item_list * next) {
  struct glob_item_list * res = new_glob_item_list_ptr();
  res -> data = data;
  res -> next = next;
  return res;
}

void print_binop(enum BinOpType op) {
  switch (op) {
  case T_PLUS:
    printf("PLUS");
    break;
  case T_MINUS:
    printf("MINUS");
    break;
  case T_MUL:
    printf("MUL");
    break;
  case T_DIV:
    printf("DIV");
    break;
  case T_MOD:
    printf("MOD");
    break;
  case T_LT:
    printf("LT");
    break;
  case T_GT:
    printf("GT");
    break;
  case T_LE:
    printf("LE");
    break;
  case T_GE:
    printf("GE");
    break;
  case T_EQ:
    printf("EQ");
    break;
  case T_NE:
    printf("NE");
    break;
  case T_AND:
    printf("AND");
    break;
  case T_OR:
    printf("OR");
    break;
  }
}

void print_unop(enum UnOpType op) {
  switch (op) {
  case T_UMINUS:
    printf("UMINUS");
    break;
  case T_NOT:
    printf("NOT");
    break;
  }
}

void print_expr(struct expr * e) {
  switch (e -> t) {
  case T_CONST:
    printf("CONST(%d)", e -> d.CONST.value);
    break;
  case T_VAR:
    printf("%s", e -> d.VAR.name);
    break;
  case T_BINOP:
    print_binop(e -> d.BINOP.op);
    printf("(");
    print_expr(e -> d.BINOP.left);
    printf(",");
    print_expr(e -> d.BINOP.right);
    printf(")");
    break;
  case T_UNOP:
    print_unop(e -> d.UNOP.op);
    printf("(");
    print_expr(e -> d.UNOP.arg);
    printf(")");
    break;
  case T_DEREF:
    printf("DEREF(");
    print_expr(e -> d.DEREF.arg);
    printf(")");
    break;
  case T_ADDR_OF:
    printf("ADDR_OF(");
    print_expr(e -> d.ADDR_OF.arg);
    printf(")");
    break;
  case T_FUNC:
    printf("FUNC(%s", e -> d.FUNC.name);
    print_expr_list(e -> d.FUNC.args);
    printf(")");
    break;
  case T_RI:
    printf("READINT");
    break;
  case T_RC:
    printf("READCHAR");
    break;
  }
}

void print_expr_list(struct expr_list * es) {
  if (es == NULL) {
    return;
  }
  printf(",");
  print_expr(es -> data);
  print_expr_list(es -> next);
}

void print_cmd(struct cmd * c) {
  int num, num1, num2;
  switch (c -> t) {
  case T_LOCAL:
    printf("LOCAL(");
    print_cmd(c -> d.LOCAL.body);
    printf(")");
    break;
  case T_DECL:
    printf("DECL(");
    printf("INT ");
    num = c -> d.DECL.num_of_ptr;
    for(int i = 0; i < num; i++)
    {
      printf("PTR ");
    }
    printf("%s", c -> d.DECL.name);
    printf(")");
    break;
  case T_DECL_ASGN:
    printf("DECL_ASGN(");
    printf("INT ");
    num1 = c -> d.DECL_ASGN.num_of_ptr;
    for(int i = 0; i < num1; i++)
    {
      printf("PTR ");
    }
    printf("%s,", c -> d.DECL_ASGN.name);
    print_expr(c -> d.DECL_ASGN.right);
    printf(")");
    break;
  case T_REF_DECL_ASGN:
    printf("REF_DECL_ASGN(");
    printf("INT ");
    num2 = c -> d.REF_DECL_ASGN.num_of_ptr;
    for(int i = 0; i < num2; i++)
    {
      printf("PTR ");
    }
    printf("REF ");
    printf("%s,%s", c -> d.REF_DECL_ASGN.name, c -> d.REF_DECL_ASGN.right);
    printf(")");
    break;
  case T_ASGN:
    printf("ASGN(");
    print_expr( c -> d.ASGN.left);
    printf(",");
    print_expr(c -> d.ASGN.right);
    printf(")");
    break;
  case T_SEQ:
    printf("SEQ(");
    print_cmd(c -> d.SEQ.left);
    printf(",");
    print_cmd(c -> d.SEQ.right);
    printf(")");
    break;
  case T_IF:
    printf("IF(");
    print_expr(c -> d.IF.cond);
    printf(",");
    print_cmd(c -> d.IF.left);
    printf(",");
    print_cmd(c -> d.IF.right);
    printf(")");
    break;
  case T_WHILE:
    printf("WHILE(");
    print_expr(c -> d.WHILE.cond);
    printf(",");
    print_cmd(c -> d.WHILE.body);
    printf(")");
    break;
  case T_FOR:
    printf("FOR(");
    print_cmd(c -> d.FOR.init);
    printf(",");
    print_expr(c -> d.FOR.cond);
    printf(",");
    print_cmd(c -> d.FOR.incr);
    printf(",");
    print_cmd(c -> d.FOR.body);
    printf(")");
    break;
  case T_DO_WHILE:
    printf("DO_WHILE(");
    print_cmd(c -> d.DO_WHILE.body);
    printf(",");
    print_expr(c -> d.DO_WHILE.cond);
    printf(")");
    break;
  case T_PROC:
    printf("PROC(%s", c -> d.PROC.name);
    print_expr_list(c -> d.PROC.args);
    printf(")");
    break;
  case T_BREAK:
    printf("BREAK");
    break;
  case T_CONTINUE:
    printf("CONTINUE");
    break;
  case T_WI:
    printf("WRITEINT(");
    print_expr(c -> d.WI.arg);
    printf(")");
    break;
  case T_WC:
    printf("WRITECHAR(");
    print_expr(c -> d.WC.arg);
    printf(")");
    break;
  }
}

void _print_var_list(struct var_list * vs) {
  if (vs == NULL) {
    return;
  }
  printf(",");
  printf("INT ");
  int num = vs -> num_of_ptr;
  for(int i = 0; i < num; i++)
  {
    printf("PTR ");
  }
  if(vs -> is_ref == 1)
  {
    printf("REF ");
  }
  printf("%s", vs -> name);
  _print_var_list(vs -> next);
}

void print_var_list(struct var_list * vs) {
  if (vs == NULL) {
    return;
  }
  printf("INT ");
  int num = vs -> num_of_ptr;
  for(int i = 0; i < num; i++)
  {
    printf("PTR ");
  }
  if(vs -> is_ref == 1)
  {
    printf("REF ");
  }
  printf("%s", vs -> name);
  _print_var_list(vs -> next);
}

void print_glob_item(struct glob_item * g) {
  switch (g -> t) {
  case T_FUNC_DEF:
    printf("FUNC %s(", g -> d.FUNC_DEF.name);
    print_var_list(g -> d.FUNC_DEF.args);
    printf(")\n  ");
    print_cmd(g -> d.FUNC_DEF.body);
    printf("\n\n");
    return;
  case T_PROC_DEF:
    printf("PROC %s(", g -> d.PROC_DEF.name);
    print_var_list(g -> d.PROC_DEF.args);
    printf(")\n  ");
    print_cmd(g -> d.PROC_DEF.body);
    printf("\n\n");
    return;
  case T_GLOB_VAR:
    printf("INT ");
    int num = g -> d.GLOB_VAR.num_of_ptr;
    for(int i = 0; i < num; i++)
    {
      printf("PTR ");
    }
    printf("%s\n\n", g -> d.GLOB_VAR.name);
    return;
  }
}

void print_glob_item_list(struct glob_item_list * gs) {
  if (gs == NULL) {
    return;
  }
  print_glob_item(gs -> data);
  print_glob_item_list(gs -> next);
}