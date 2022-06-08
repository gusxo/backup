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

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
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
    ID = 258,
    STRING = 259,
    INCLUDEPATH = 260,
    NUMBER = 261,
    CHAR = 262,
    LEFTROUNDBRACKET = 263,
    RIGHTROUNDBRACKET = 264,
    LEFTCRULYBRACKET = 265,
    RIGHTCRULYBRACKET = 266,
    LEFTBOXBRACKET = 267,
    RIGHTBOXBRACKET = 268,
    LEFTANGLEBRACKET = 269,
    RIGHTANGLEBRACKET = 270,
    SHARP = 271,
    COMMA = 272,
    SEMICOLON = 273,
    RETURN = 274,
    INCLUDE = 275,
    WHILE = 276,
    IF = 277,
    CHARTYPE = 278,
    INTTYPE = 279,
    REF = 280,
    PLUS = 281,
    MINUS = 282,
    MUL = 283,
    DIV = 284,
    EQ = 285
  };
#endif
/* Tokens.  */
#define ID 258
#define STRING 259
#define INCLUDEPATH 260
#define NUMBER 261
#define CHAR 262
#define LEFTROUNDBRACKET 263
#define RIGHTROUNDBRACKET 264
#define LEFTCRULYBRACKET 265
#define RIGHTCRULYBRACKET 266
#define LEFTBOXBRACKET 267
#define RIGHTBOXBRACKET 268
#define LEFTANGLEBRACKET 269
#define RIGHTANGLEBRACKET 270
#define SHARP 271
#define COMMA 272
#define SEMICOLON 273
#define RETURN 274
#define INCLUDE 275
#define WHILE 276
#define IF 277
#define CHARTYPE 278
#define INTTYPE 279
#define REF 280
#define PLUS 281
#define MINUS 282
#define MUL 283
#define DIV 284
#define EQ 285

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 9 "yc.y"

        char* str;
        unsigned int num;

#line 122 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
