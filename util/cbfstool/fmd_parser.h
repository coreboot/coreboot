/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 34 "fmd_parser.y" /* yacc.c:1909  */

#include "fmd.h"
#include "option.h"

#include <stdbool.h>

struct descriptor_node {
	struct flashmap_descriptor *val;
	struct descriptor_node *next;
};

struct descriptor_list {
	size_t len;
	struct descriptor_node *head;
	struct descriptor_node *tail;
};

extern struct flashmap_descriptor *res;

struct flashmap_descriptor *parse_descriptor(
	char *name, union flashmap_flags flags, struct unsigned_option offset,
	struct unsigned_option size, struct descriptor_list children);
void yyerror(const char *s);

#line 69 "y.tab.h" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INTEGER = 258,
    OCTAL = 259,
    STRING = 260,
    FLAG_CBFS = 261,
    FLAG_PRESERVE = 262
  };
#endif
/* Tokens.  */
#define INTEGER 258
#define OCTAL 259
#define STRING 260
#define FLAG_CBFS 261
#define FLAG_PRESERVE 262

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 25 "fmd_parser.y" /* yacc.c:1909  */

	unsigned intval;
	char *strval;
	struct unsigned_option maybe_intval;
	struct flashmap_descriptor *region_ptr;
	union flashmap_flags flags;
	struct descriptor_list region_listhdr;

#line 104 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
