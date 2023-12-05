%{
	// this part is copied to the beginning of the parser 
	#include <stdio.h>
	#include "lang.h"
	#include "lexer.h"
	void yyerror(char *);
	int yylex(void);
  struct glob_item_list * root; // Now the root is a pointer to the global item list
%}

%union {
unsigned int n;
char * i;
struct expr * e;
struct cmd * c;

// newly add one: add all the struct defined in lang.h to the union
struct glob_item * globitem;
struct var_list * varl;
struct expr_list * exprl;
struct glob_item_list * globiteml;
//

void * none;
}

// Terminals:
%token <n> TM_NAT
%token <i> TM_IDENT
%token <none> TM_LEFT_BRACE TM_RIGHT_BRACE
%token <none> TM_LEFT_PAREN TM_RIGHT_PAREN
%token <none> TM_SEMICOL

//,
%token <none> TM_COMMA;
//

%token <none> TM_MALLOC TM_RI TM_RC TM_WI TM_WC
%token <none> TM_VAR TM_IF TM_THEN TM_ELSE TM_WHILE TM_DO TM_FOR
%token <none> TM_ASGNOP
%token <none> TM_OR
%token <none> TM_AND
%token <none> TM_NOT

//&
%token <none> TM_ADDR
//

%token <none> TM_LT TM_LE TM_GT TM_GE TM_EQ TM_NE
%token <none> TM_PLUS TM_MINUS
%token <none> TM_MUL TM_DIV TM_MOD
%token <none> TM_UMINUS TM_DEREF
//func and proc 
%token <none> TM_PROC TM_FUNC
//continue and break and return
%token <none> TM_CONT TM_BREAK TM_RET 

//This is what I added: I delete TM_VAR and add TM_INT / TM_PTR for the type system
%token <none> TM_INT
%token <none> TM_PTR

// Nonterminals

//newly added: NT_WHOLE is the whole program which is a list of global item
//NT_GLOBALS is actually a set of global items
// NT_GlOBAL is one global item: var definition or proc or func
%type <globiteml> NT_WHOLE
%type <globiteml> NT_GLOBALS
%type <globitem> NT_GLOBAL
//


%type <c> NT_CMD
%type <e> NT_EXPR0
%type <e> NT_EXPR1
%type <e> NT_EXPR


//This is what I added: I add NT_PTRS to represent multiple TM_PTRS
%type <e> NT_PTRS

// NT_EXPRL is list of expressions used in calling the proc
// NT_VARL is list of variables used in defining the func and proc
%type <exprl> NT_EXPRL
%type <varl> NT_VARL
//

// Priority: the lower the greater
%nonassoc TM_ASGNOP TM_BREAK TM_CONT TM_RET// = break continue return
%left TM_OR // ||
%left TM_AND // &&
%left TM_LT TM_LE TM_GT TM_GE TM_EQ TM_NE // > < 
%left TM_PLUS TM_MINUS // + -
%left TM_MUL TM_DIV TM_MOD // * /  %
%right TM_UMINUS TM_DEREF TM_ADDR// - *
%left TM_NOT // !
%left TM_LEFT_PAREN TM_RIGHT_PAREN //( )
%right TM_COMMA
%right TM_SEMICOL//;

%%

NT_WHOLE:
  NT_GLOBALS
  {
    $$ = ($1);
    root = $$; //root is the final root of the tree
  } 
;

//
NT_GLOBALS://similar to E -> F       E -> F , E to create multiple GLOBAL ITEM
  NT_GLOBAL
  {
    $$ = (TGCons($1, TGNil())); // This TGCons function update the global item list
    //struct glob_item_list * TGCons(struct glob_item * data,  struct glob_item_list * next);
  }
| NT_GLOBAL TM_SEMICOL NT_GLOBALS
  {
    $$ = (TGCons($1, $3));
  }
;


NT_GLOBAL:
  TM_VAR TM_IDENT
  {
    $$ = (TGlobVar($2));//This function create the global variable
    //struct glob_item * TGlobVar(char * name);
  }
| TM_FUNC TM_IDENT TM_LEFT_PAREN NT_VARL TM_RIGHT_PAREN TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE 
  {
    $$ = (TFuncDef($2, $4, $7));//This function define the free function
    //struct glob_item * TFuncDef(char * name, struct var_list * args, struct cmd * body);
  }
| TM_FUNC TM_IDENT TM_LEFT_PAREN TM_RIGHT_PAREN TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE 
  {
    $$ = (TFuncDef($2, TVNil(), $6)); //The one with no parameter
  }
| TM_PROC TM_IDENT TM_LEFT_PAREN NT_VARL TM_RIGHT_PAREN TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE 
  {
    $$ = (TProcDef($2, $4, $7));//This function define the process
    //struct glob_item * TProcDef(char * name, struct var_list * args, struct cmd * body);
  }
| TM_PROC TM_IDENT TM_LEFT_PAREN TM_RIGHT_PAREN TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE 
  {
    $$ = (TProcDef($2, TVNil(), $6));//The one with no parameter
  }
;

NT_VARL:
  TM_IDENT
  {
    $$ = (TVCons($1, TVNil()));
    //struct var_list * TVCons(char * name, struct var_list * next);
  }
| TM_IDENT TM_COMMA NT_VARL
  {
    $$ = (TVCons($1, $3));
  }
;

NT_EXPRL:
  NT_EXPR
  {
    $$ = (TECons($1, TENil()));
  }
| NT_EXPR TM_COMMA NT_EXPRL
  {
    $$ = (TECons($1, $3));
    //struct glob_item_list * TGCons(struct glob_item * data, struct glob_item_list * next);
  }
;


//This is what I added:
NT_PTRS:
  NT_PTRS TM_PTR
  {
    $$ = (T);
  }
//

NT_CMD:
 //This is what I modified:
//Now we can convert a variable declaration command into
//TM_INT TM_IDENT or TM_INT NT_PTRS TM__IDENT
  TM_VAR TM_IDENT TM_SEMICOL NT_CMD//declare a variable and the rest command
  {
    $$ = (TDecl($2, $4));
  }
| TM_INT TM_IDENT TM_SEMICOL NT_CMD//declare a variable and the rest command
  {
    $$ = (TDecl($2, $4));
  }
| TM_INT NT_PTRS TM_IDENT TM_SEMICOL NT_CMD
  {
    $$ = (TDeclptrs($3, $5));
  }
//
| NT_EXPR TM_ASGNOP NT_EXPR//assignment
  {
    $$ = (TAsgn($1, $3));
  }
| NT_CMD TM_SEMICOL NT_CMD//sequential sentences
  {
    $$ = (TSeq($1, $3));
  }
| TM_IF TM_LEFT_PAREN NT_EXPR TM_RIGHT_PAREN TM_THEN TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE TM_ELSE TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE
  { // if sentence including a condition expression and two commands
    $$ = (TIf($3, $7, $11));
  }
| TM_WHILE TM_LEFT_PAREN NT_EXPR TM_RIGHT_PAREN TM_DO TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE
  {// while sentence
    $$ = (TWhile($3, $7)); 
  }
//This is special : for 
//struct cmd * TFor(struct cmd * init, struct expr * cond, struct cmd * incr, struct cmd * body);
| TM_FOR TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE TM_LEFT_PAREN NT_EXPR TM_RIGHT_PAREN TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE
  {
    $$ = (TFor($3, $6, $9, $12));
  }
//This is special: do while
//struct cmd * TDoWhile(struct cmd * body, struct expr * cond);
| TM_DO TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE TM_WHILE TM_LEFT_PAREN NT_EXPR TM_RIGHT_PAREN
  {
    $$ = (TDoWhile($3, $7));
  }
// continue break and return
| TM_CONT
  {
    $$ = (TContinue());
  }
| TM_BREAK
  {
    $$ = (TBreak());
  }
| TM_RET
  {
    $$ = (TReturn());
  }
//call a proc
| TM_IDENT TM_LEFT_PAREN NT_EXPRL TM_RIGHT_PAREN
  {
    $$ = (TProc($1, $3));
  }
/* struct cmd * TProc(char * name, struct expr_list * args);
struct cmd * TBreak();
struct cmd * TContinue();
struct cmd * TReturn(); */

// take the one with no paramater as a special case
| TM_IDENT TM_LEFT_PAREN TM_RIGHT_PAREN
  {
    $$ = (TProc($1, TENil()));
  }
| TM_WI TM_LEFT_PAREN NT_EXPR TM_RIGHT_PAREN
  {
    $$ = (TWriteInt($3));
  }
| TM_WC TM_LEFT_PAREN NT_EXPR TM_RIGHT_PAREN
  {
    $$ = (TWriteChar($3));
  }
; 


NT_EXPR0:
  TM_NAT
  {
    $$ = (TConst($1));
  }
| TM_LEFT_PAREN NT_EXPR TM_RIGHT_PAREN
  {
    $$ = ($2);
  }
| TM_IDENT
  {
    $$ = (TVar($1));
  }
;

NT_EXPR1:
  TM_NOT NT_EXPR
  {
    $$ = (TUnOp(T_NOT, $2));
  }
| NT_EXPR0
  {
    $$ = ($1);
  }
;


NT_EXPR:
  NT_EXPR1
  {
    $$ = ($1);
  }
| TM_MINUS NT_EXPR %prec TM_UMINUS
  {
    $$ = (TUnOp(T_UMINUS, $2));
  }
| TM_MUL NT_EXPR  %prec TM_DEREF
  {
    $$ = (TDeref($2));
  }

//
| TM_ADDR NT_EXPR
  {
    $$ = (TAddrOf($2));
  }
//

| NT_EXPR TM_MUL NT_EXPR
  {
    $$ = (TBinOp(T_MUL, $1, $3));
  }
| NT_EXPR TM_PLUS NT_EXPR
  {
    $$ = (TBinOp(T_PLUS, $1, $3));
  }
| NT_EXPR TM_MINUS NT_EXPR
  {
    $$ = (TBinOp(T_MINUS, $1, $3));
  }
| NT_EXPR TM_DIV NT_EXPR
  {
    $$ = (TBinOp(T_DIV, $1, $3));
  }
| NT_EXPR TM_MOD NT_EXPR
  {
    $$ = (TBinOp(T_MOD, $1, $3));
  }
| NT_EXPR TM_LT NT_EXPR
  {
    $$ = (TBinOp(T_LT, $1, $3));
  }
| NT_EXPR TM_GT NT_EXPR
  {
    $$ = (TBinOp(T_GT, $1, $3));
  }
| NT_EXPR TM_LE NT_EXPR
  {
    $$ = (TBinOp(T_LE, $1, $3));
  }
| NT_EXPR TM_GE NT_EXPR
  {
    $$ = (TBinOp(T_GE, $1, $3));
  }
| NT_EXPR TM_EQ NT_EXPR
  {
    $$ = (TBinOp(T_EQ, $1, $3));
  }
| NT_EXPR TM_NE NT_EXPR
  {
    $$ = (TBinOp(T_NE, $1, $3));
  }
| NT_EXPR TM_AND NT_EXPR
  {
    $$ = (TBinOp(T_AND, $1, $3));
  }
| NT_EXPR TM_OR NT_EXPR
  {
    $$ = (TBinOp(T_OR, $1, $3));
  }
| TM_IDENT TM_LEFT_PAREN NT_EXPRL TM_RIGHT_PAREN
  {
    $$ = (TFunc($1, $3));
    //struct expr * TFunc(char * name, struct expr_list * args);
  }
| TM_IDENT TM_LEFT_PAREN TM_RIGHT_PAREN
  {
    $$ = (TFunc($1, TENil()));
  }
| TM_MALLOC TM_LEFT_PAREN NT_EXPR TM_RIGHT_PAREN
  {
    $$ = (TMalloc($3));
  }
| TM_RI TM_LEFT_PAREN TM_RIGHT_PAREN
  {
    $$ = (TReadInt());
  }
| TM_RC TM_LEFT_PAREN TM_RIGHT_PAREN
  {
    $$ = (TReadChar());
  }
;


%%

void yyerror(char* s)
{
    fprintf(stderr , "%s\n",s);
}
