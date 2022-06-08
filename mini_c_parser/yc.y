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
%token COMMA SEMICOLON RETURN INCLUDE WHILE IF CHARTYPE INTTYPE
%left PLUS MINUS MUL DIV EQ

%type <str> define_variable assignment variable

%%

statement       : SEMICOLON
                {}
                | define_variable SEMICOLON
                {free($1);}
                | assignment SEMICOLON
                {}
                | expression SEMICOLON
                {}
                | include
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
                | ID EQ expression
                {printf("assign to %s\n", $1); free($1);}
                ;

expression      : mul_expression
                {}
                | expression PLUS mul_expression
                {printf("stack : +\n");}
                | expression MINUS mul_expression
                {printf("stack : -\n");}
                ;

mul_expression  : variable
                {}
                | mul_expression MUL variable
                {printf("stack : *\n");}
                | mul_expression DIV variable
                {printf("stack : /\n");}
                ;

variable        : LEFTROUNDBRACKET expression RIGHTROUNDBRACKET
                {}
                | ID
                {printf("use variable : %s\n", $1);}
                | NUMBER
                {printf("use constant : %u\n", $1);}
                | CHAR
                {printf("use constant : %c\n", $1);}
                | STRING
                {printf("use constant : %s\n", $1);}
                ;

%%
int main(){
        yyparse();
}
int yyerror(char *s)
{
  fprintf(stderr, "%d : %s\n", line, s);
}