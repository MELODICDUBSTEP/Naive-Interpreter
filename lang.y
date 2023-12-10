%{
	#include <stdio.h>
	#include "lang.h"
	#include "lexer.h"
	void yyerror(char *);
	int yylex(void);
  struct glob_item_list * root; 
  // Now the root is a pointer to the global item list
%}

%union {
unsigned int n;
char * i;
struct expr * e;
struct cmd * c;
struct glob_item * globitem;
struct var_list * varl;
struct expr_list * exprl;
struct glob_item_list * globiteml;
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
%token <none> TM_INT TM_IF TM_THEN TM_ELSE TM_WHILE TM_DO TM_FOR
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
%token <none> TM_PROC TM_FUNC
%token <none> TM_CONT TM_BREAK 
%token <n> TM_PTR
%token <none> TM_REF

// Nonterminals

//newly added: NT_WHOLE is the whole program which is a list of global item
//NT_GLOBALS is actually a set of global items
// NT_GlOBAL is one global item: var definition or proc or func
%type <globiteml> NT_WHOLE
%type <globiteml> NT_GLOBALS
%type <globitem> NT_GLOBAL

%type <c> NT_CMD
%type <e> NT_EXPR0
%type <e> NT_EXPR1
%type <e> NT_EXPR

//I added NT_PTRS to represent multiple TM_PTR
%type <n> NT_PTRS

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
%right TM_UMINUS TM_DEREF TM_ADDR// - * & 
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
NT_GLOBALS:
  NT_GLOBAL
  {
    $$ = (TGCons($1, TGNil())); 
  }
| NT_GLOBAL TM_SEMICOL NT_GLOBALS
  {
    $$ = (TGCons($1, $3));
  }
;


NT_GLOBAL:
  TM_INT TM_IDENT
  {
    $$ = (TGlobVar($2, 0));
  }
| TM_INT NT_PTRS TM_IDENT
  {
    $$ = (TGlobVar($3, $2));
  }
| TM_FUNC TM_INT TM_IDENT TM_LEFT_PAREN NT_VARL TM_RIGHT_PAREN TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE 
  {
    $$ = (TFuncDef($3, $5, $8));
  }
| TM_FUNC TM_INT NT_PTRS TM_IDENT TM_LEFT_PAREN NT_VARL TM_RIGHT_PAREN TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE 
  {
    $$ = (TFuncDef($4, $6, $9));
  }
| TM_FUNC TM_IDENT TM_LEFT_PAREN TM_RIGHT_PAREN TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE 
  {
    $$ = (TFuncDef($2, TVNil(), $6)); //The one with no parameter
  }
| TM_PROC TM_IDENT TM_LEFT_PAREN NT_VARL TM_RIGHT_PAREN TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE 
  {
    $$ = (TProcDef($2, $4, $7));
  }
| TM_PROC TM_IDENT TM_LEFT_PAREN TM_RIGHT_PAREN TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE 
  {
    $$ = (TProcDef($2, TVNil(), $6));//The one with no parameter
  }
;

NT_VARL:
  TM_INT TM_IDENT
  {
    $$ = (TVCons($2, TVNil(), 0, 0));
  }// int a
| TM_INT NT_PTRS TM_IDENT
  {
    $$ = (TVCons($3, TVNil(), $2, 0));
  } // int ptr ... ptr a
| TM_INT TM_REF TM_IDENT
  {
    $$ = (TVCons($3, TVNil(), 0, 1));
  } // int ref a
| TM_INT NT_PTRS TM_REF TM_IDENT
  {
    $$ = (TVCons($4, TVNil(), $2, 1));
  } // int ptr ... ptr ref a
| TM_INT NT_PTRS TM_IDENT TM_COMMA NT_VARL
  {
    $$ = (TVCons($3, $5, $2, 0));
  } //int ptr ... ptr a , ... 
| TM_INT NT_PTRS TM_REF TM_IDENT TM_COMMA NT_VARL
  {
    $$ = (TVCons($4, $6, $2, 1));
  } //int ptr ... ptr ref a , ... 
| TM_INT TM_IDENT TM_COMMA NT_VARL
  {
    $$ = (TVCons($2, $4, 0, 0));
  } //int a, ...
| TM_INT TM_REF TM_IDENT TM_COMMA NT_VARL
  {
    $$ = (TVCons($3, $5, 0, 1));
  } // int ref a, ...
;

NT_EXPRL:
  NT_EXPR
  {
    $$ = (TECons($1, TENil()));
  }
| NT_EXPR TM_COMMA NT_EXPRL
  {
    $$ = (TECons($1, $3));
  }
;

//Now NT_PTRS will help me store the number of layers(as an integer)
NT_PTRS:
  TM_PTR
  {
    $$ = 1;
  }
| NT_PTRS TM_PTR
  {
    $$ = $1 + 1;
  }

NT_CMD:
  TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE
  {
    $$ = (TLocal($2));
  }
| TM_INT TM_IDENT TM_SEMICOL NT_CMD
  {
    $$ = (TDecl($2, $4, 0));
  } //int a; ...
| TM_INT TM_IDENT TM_ASGNOP NT_EXPR TM_SEMICOL NT_CMD
  {
    $$ = (TDeclAsgn($2, $6, 0, $4));
  } //int a = b; ...
    //int a = 1; ...
| TM_INT TM_REF TM_IDENT TM_ASGNOP TM_IDENT TM_SEMICOL NT_CMD
  {
    $$ = (TRefDeclAsgn($3, $7, 0, $5));
  } //int ref a = b; ...
| TM_INT NT_PTRS TM_IDENT TM_SEMICOL NT_CMD
  {
    $$ = (TDecl($3, $5, $2));
  } //int ptr ... ptr a; ...
| TM_INT NT_PTRS TM_IDENT TM_ASGNOP NT_EXPR TM_SEMICOL NT_CMD
  {
    $$ = (TDeclAsgn($3, $7, $2, $5));
  } //int ptr ... ptr a = b; ...
    //int ptr ... ptr a = 1; ...
| TM_INT NT_PTRS TM_REF TM_IDENT TM_ASGNOP TM_IDENT TM_SEMICOL NT_CMD
  {
    $$ = (TRefDeclAsgn($4, $8, $2, $6));
  } //int ptr ... ptr ref a = b; ...
| NT_EXPR TM_ASGNOP NT_EXPR
  {
    $$ = (TAsgn($1, $3));
  } //a = b
    //a = 1
| NT_CMD TM_SEMICOL NT_CMD//sequential sentences
  {
    $$ = (TSeq($1, $3));
  } //C;C
| TM_IF TM_LEFT_PAREN NT_EXPR TM_RIGHT_PAREN TM_THEN TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE TM_ELSE TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE
  { 
    $$ = (TIf($3, $7, $11));
  }
| TM_WHILE TM_LEFT_PAREN NT_EXPR TM_RIGHT_PAREN TM_DO TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE
  {
    $$ = (TWhile($3, $7)); 
  }
| TM_FOR TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE TM_LEFT_PAREN NT_EXPR TM_RIGHT_PAREN TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE
  {
    $$ = (TFor($3, $6, $9, $12));
  }
| TM_DO TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE TM_WHILE TM_LEFT_PAREN NT_EXPR TM_RIGHT_PAREN
  {
    $$ = (TDoWhile($3, $7));
  }
| TM_CONT
  {
    $$ = (TContinue());
  }
| TM_BREAK
  {
    $$ = (TBreak());
  }
//call a proc
| TM_IDENT TM_LEFT_PAREN NT_EXPRL TM_RIGHT_PAREN
  {
    $$ = (TProc($1, $3));
  }
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