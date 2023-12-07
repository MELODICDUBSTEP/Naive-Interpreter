#ifndef LANG_H_INCLUDED
#define LANG_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "lib.h"

enum BinOpType {
  T_PLUS,
  T_MINUS,
  T_MUL,
  T_DIV,
  T_MOD,
  T_LT,
  T_GT,
  T_LE,
  T_GE,
  T_EQ,
  T_NE,
  T_AND,
  T_OR
};

enum UnOpType {
  T_UMINUS,
  T_NOT
};

enum ExprType {
  T_CONST = 0,
  T_VAR,
  T_BINOP,
  T_UNOP,
  T_DEREF,
  T_ADDR_OF,
  T_FUNC,
  T_MALLOC,
  T_RI,
  T_RC
};

enum CmdType {
  T_DECL = 0,
  T_DECL_ASGN,
  T_REF_DECL_ASGN,
  T_ASGN,
  T_SEQ,
  T_IF,
  T_WHILE,
  T_FOR,
  T_DO_WHILE,
  T_PROC,
  T_WI,
  T_WC,
  T_BREAK,
  T_CONTINUE,
  T_RETURN
};

enum GlobItemType {
  T_FUNC_DEF,
  T_PROC_DEF,
  T_GLOB_VAR
};

struct expr;

struct expr_list {
  struct expr * data;
  struct expr_list * next;
};

struct expr {
  enum ExprType t;
  union {
    struct {unsigned int value; } CONST;
    struct {char * name; int num_of_ptr; int is_ref; } VAR;
    struct {enum BinOpType op; struct expr * left; struct expr * right; } BINOP;
    struct {enum UnOpType op; struct expr * arg; } UNOP;
    struct {struct expr * arg; } DEREF;
    struct {struct expr * arg; } ADDR_OF;
    struct {char * name; struct expr_list * args; } FUNC;
    struct {struct expr * arg; } MALLOC;
    struct {void * none; } RI;
    struct {void * none; } RC;
  } d;
};

/*
I added DECL_ASGN to represent the command : type V = E
And I added REF_DECL_ASGN to represent the command : type ref V = v
*/

struct cmd {
  enum CmdType t;
  union {
    struct {char * name; struct cmd * body; int num_of_ptr; } DECL;
    struct {char * name; struct cmd * body; int num_of_ptr; struct expr * right; } DECL_ASGN;
    struct {char * name; struct cmd * body; int num_of_ptr; char * right; } REF_DECL_ASGN;
    struct {struct expr * left; struct expr * right; } ASGN;
    struct {struct cmd * left; struct cmd * right; } SEQ;
    struct {struct expr * cond; struct cmd * left; struct cmd * right; } IF;
    struct {struct expr * cond; struct cmd * body; } WHILE;
    struct {struct cmd * init; struct expr * cond;
            struct cmd * incr; struct cmd * body; } FOR;
    struct {struct cmd * body; struct expr * cond; } DO_WHILE;
    struct {char * name; struct expr_list * args; } PROC;
    struct {void * none; } BREAK;
    struct {void * none; } CONTINUE;
    struct {void * none; } RETURN;
    struct {struct expr * arg; } WI;
    struct {struct expr * arg; } WC;
  } d;
};


/*Now my variable has three atributes : 
1.name 
2. num_of_ptr, to represent how many layers of ptr are there of this variable
3. is_ref, to represent whether or not this variable is a reference
*/

struct var_list {
  char * name;
  int num_of_ptr;
  int is_ref;
  struct var_list * next;
};

struct glob_item {
  enum GlobItemType t;
  union {
    struct {char * name; struct var_list * args; struct cmd * body; } FUNC_DEF;
    struct {char * name; struct var_list * args; struct cmd * body; } PROC_DEF;
    struct {char * name; int num_of_ptr; } GLOB_VAR;
  } d;
};

struct glob_item_list {
  struct glob_item * data;
  struct glob_item_list * next;
};

struct expr_list * TENil();
struct expr_list * TECons(struct expr * data, struct expr_list * next);
struct expr * TConst(unsigned int value);
struct expr * TVar(char * name);
struct expr * TBinOp(enum BinOpType op, struct expr * left,
                     struct expr * right);
struct expr * TUnOp(enum UnOpType op, struct expr * arg);
struct expr * TDeref(struct expr * arg);
struct expr * TAddrOf(struct expr * arg);
struct expr * TMalloc(struct expr * arg);
struct expr * TReadInt();
struct expr * TReadChar();
struct expr * TFunc(char * name, struct expr_list * args);
struct cmd * TDecl(char * name, struct cmd * body, int num_of_ptr);
struct cmd * TDeclAsgn(char * name, struct cmd * body, int num_of_ptr, struct expr * right);
struct cmd * TRefDeclAsgn(char * name, struct cmd * body, int num_of_ptr, char * right);
struct cmd * TAsgn(struct expr * left, struct expr * right);
struct cmd * TSeq(struct cmd * left, struct cmd * right);
struct cmd * TIf(struct expr * cond, struct cmd * left, struct cmd * right);
struct cmd * TWhile(struct expr * cond, struct cmd * body);
struct cmd * TFor(struct cmd * init, struct expr * cond,
                  struct cmd * incr, struct cmd * body);
struct cmd * TDoWhile(struct cmd * body, struct expr * cond);
struct cmd * TProc(char * name, struct expr_list * args);
struct cmd * TBreak();
struct cmd * TContinue();
struct cmd * TReturn();
struct var_list * TVNil();
struct var_list * TVCons(char * name, struct var_list * next, int num_of_ptr, int is_ref);
struct glob_item * TGlobVar(char * name, int num_of_ptr);
struct glob_item * TFuncDef(char * name, struct var_list * args,
                            struct cmd * body);
struct glob_item * TProcDef(char * name, struct var_list * args,
                            struct cmd * body);
struct glob_item_list * TGNil();
struct glob_item_list * TGCons(struct glob_item * data,
                               struct glob_item_list * next);

struct cmd * TWriteInt(struct expr * arg);
struct cmd * TWriteChar(struct expr * arg);

void print_binop(enum BinOpType op);
void print_unop(enum UnOpType op);
void print_expr(struct expr * e);
void print_expr_list(struct expr_list * es);
void print_cmd(struct cmd * c);
void print_var_list(struct var_list * vs);
void print_glob_item(struct glob_item * g);
void print_glob_item_list(struct glob_item_list * gs);

#endif // LANG_H_INCLUDED