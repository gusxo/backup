%{
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
int line;
int yylex(void);
int yyerror(char* s);
%}
%union{
        char* str;
        unsigned int num;
}

%token <str> ID STRING INCLUDEPATH
%token <num> NUMBER CHAR
%token LEFTROUNDBRACKET RIGHTROUNDBRACKET LEFTCRULYBRACKET RIGHTCRULYBRACKET
%token LEFTBOXBRACKET RIGHTBOXBRACKET LEFTANGLEBRACKET RIGHTANGLEBRACKET SHARP
%token COMMA SEMICOLON RETURN INCLUDE WHILE IF CHARTYPE INTTYPE REF
%left PLUS MINUS MUL DIV EQ

%type <str> define_variable assignment variable

%%

start_unit      : statement
                | include start_unit
                | statement start_unit
                ;

statement       : SEMICOLON
                {}
                | define_variable SEMICOLON
                {free($1);}
                | assignment SEMICOLON
                {}
                | expression SEMICOLON
                {}
                ;

include         : SHARP INCLUDE INCLUDEPATH
                {printf("include standard library : %s\n", $3); free($3);}
                | SHARP INCLUDE STRING
                {printf("include user library : %s\n", $3 ); free($3);}
                ;

define_variable : INTTYPE ID
                {printf("def int %s\n", $2); $$ = $2;}
                | CHARTYPE ID
                {printf("def char %s\n", $2); $$ = $2;}
                | INTTYPE MUL ID
                {printf("def int* %s\n", $3); $$ = $3;}
                | CHARTYPE MUL ID
                {printf("def char* %s\n", $3); $$ = $3;}
                ;

assignment      : define_variable EQ expression
                {printf("assign to %s\n", $1); free($1);}
                | variable EQ expression
                {printf("assign\n"); free($1);}
                ;

function_arg    : expression
                {printf("stack.pop(), add to function argument\n");}
                | expression COMMA function_arg
                {printf("stack.pop(), add to function argument\n");}
                ;

function_call   : value LEFTROUNDBRACKET RIGHTROUNDBRACKET
                {printf("function call\n");}
                | value LEFTROUNDBRACKET function_arg RIGHTROUNDBRACKET
                {printf("function call\n");}

expression      : mul_expression
                {}
                | expression PLUS mul_expression
                {printf("operation : +\n");}
                | expression MINUS mul_expression
                {printf("operation : -\n");}
                ;

mul_expression  : expression_value
                {}
                | mul_expression MUL expression_value
                {printf("operation : *\n");}
                | mul_expression DIV expression_value
                {printf("operation : /\n");}
                ;

expression_value: const_value
                {}
                | value
                {}
                ;

variable        : ID
                {printf("use variable : %s\n", $1); $$ = $1;}
                | MUL variable
                {printf("-- use pointer's reference\n"); $$ = $2;}
                | REF variable
                {printf("-- use address\n"); $$ = $2;}
                ;

const_value     : NUMBER
                {printf("use constant int : %u\n", $1);}
                | CHAR
                {printf("use constant char : %c\n", $1);}
                | STRING
                {printf("use constant string : %s\n", $1); free($1);}
                ;          

value           : LEFTROUNDBRACKET expression RIGHTROUNDBRACKET
                {}
                | variable
                {free($1);}
                | function_call
                {}
                ;
                

%%
int main(){
        yyparse();
}
int yyerror(char *s)
{
  fprintf(stderr, "%d : %s\n", line, s);
}