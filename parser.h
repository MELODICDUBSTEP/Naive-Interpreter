/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TM_NAT = 258,
    TM_IDENT = 259,
    TM_LEFT_BRACE = 260,
    TM_RIGHT_BRACE = 261,
    TM_LEFT_PAREN = 262,
    TM_RIGHT_PAREN = 263,
    TM_SEMICOL = 264,
    TM_COMMA = 265,
    TM_MALLOC = 266,
    TM_RI = 267,
    TM_RC = 268,
    TM_WI = 269,
    TM_WC = 270,
    TM_VAR = 271,
    TM_IF = 272,
    TM_THEN = 273,
    TM_ELSE = 274,
    TM_WHILE = 275,
    TM_DO = 276,
    TM_FOR = 277,
    TM_ASGNOP = 278,
    TM_OR = 279,
    TM_AND = 280,
    TM_NOT = 281,
    TM_ADDR = 282,
    TM_LT = 283,
    TM_LE = 284,
    TM_GT = 285,
    TM_GE = 286,
    TM_EQ = 287,
    TM_NE = 288,
    TM_PLUS = 289,
    TM_MINUS = 290,
    TM_MUL = 291,
    TM_DIV = 292,
    TM_MOD = 293,
    TM_UMINUS = 294,
    TM_DEREF = 295,
    TM_PROC = 296,
    TM_FUNC = 297,
    TM_CONT = 298,
    TM_BREAK = 299,
    TM_RET = 300,
    TM_INT = 301,
    TM_PTR = 302
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 11 "lang.y"

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

#line 121 "parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_H_INCLUDED  */
