%{
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
int yylex(void);
int yyerror(char* s);

//
int block_depth = 0;
void indent(){
        for(int i = 0;i<block_depth;++i)
                printf("  ");
}
        

%}
%union{
        char* str;
        unsigned int num;
}

%token <str> ID STRING INCLUDEPATH
%token <num> NUMBER CHAR
%token LEFTROUNDBRACKET RIGHTROUNDBRACKET LEFTCRULYBRACKET RIGHTCRULYBRACKET
%token LEFTBOXBRACKET RIGHTBOXBRACKET LEFTANGLEBRACKET RIGHTANGLEBRACKET SHARP
%token COMMA SEMICOLON RETURN INCLUDE WHILE IF CHARTYPE INTTYPE REF EQ_OP NEQ
%left PLUS MINUS MUL DIV EQ

%type <str> assignment variable data_types primary_types

%%

start_unit      : extern_def
                | extern_def start_unit
                ;

extern_def      : include
                | define_variable
                | function_def

statement       : SEMICOLON
                {}
                | define_variable
                {}
                | assignment SEMICOLON
                {}
                | equal_exp SEMICOLON
                {}
                | RETURN equal_exp SEMICOLON
                {indent(); printf("return value\n");}
                ;

include         : SHARP INCLUDE INCLUDEPATH
                {indent(); printf("include standard library : %s\n", $3); free($3);}
                | SHARP INCLUDE STRING
                {indent(); printf("include user library : %s\n", $3 ); free($3);}
                ;


primary_types   : INTTYPE
                {$$ = malloc(4); strcpy($$, "int");}
                | CHARTYPE
                {$$ = malloc(5); strcpy($$, "char");}

data_types      : primary_types
                {$$ = $1;}
                | data_types MUL
                {
                unsigned int len = strlen($1);
                $$ = malloc(len+2);
                strcpy($$, $1);
                $$[len] = '*';
                $$[len+1] = '\0';
                free($1);
                }

define_variable : data_types ID SEMICOLON
                {indent(); printf("def %s %s\n", $1, $2); free($1); free($2);}
                | data_types ID EQ equal_exp SEMICOLON
                {indent(); printf("def %s %s, and assign\n", $1, $2); free($1); free($2);}

assignment      : variable EQ equal_exp
                {indent(); printf("assign\n"); free($1);}
                ;

function_arg    : equal_exp
                {indent(); printf("stack.pop(), add to function argument\n");}
                | equal_exp COMMA function_arg
                {indent(); printf("stack.pop(), add to function argument\n");}
                ;

function_call   : value LEFTROUNDBRACKET RIGHTROUNDBRACKET
                {indent(); printf("function call\n");}
                | value LEFTROUNDBRACKET function_arg RIGHTROUNDBRACKET
                {indent(); printf("function call\n");}
                ;

code_block      : code_block_start code_block_term RIGHTCRULYBRACKET
                {block_depth--; indent(); printf("code block end\n");}
                | code_block_start RIGHTCRULYBRACKET
                {block_depth--; indent(); printf("code block end\n");}
                ;

code_block_start: LEFTCRULYBRACKET
                {indent(); printf("code block start\n"); block_depth++;}

code_block_term : code_block_stmt
                | code_block_stmt code_block_term

code_block_stmt : statement
                | if_block
                | while_block
                ;

function_def    : function_def_id code_block
                ;

function_def_id : data_types ID LEFTROUNDBRACKET RIGHTROUNDBRACKET
                {indent(); printf("function name : %s\n", $2); free($2);}
                | data_types ID LEFTROUNDBRACKET function_def_arg RIGHTROUNDBRACKET
                {indent(); printf("function name : %s\n", $2); free($2);}
                ;

function_def_arg: data_types ID
                {indent(); printf("function argument : %s %s\n", $1, $2); free($1); free($2);}
                | data_types ID COMMA function_def_arg
                {indent(); printf("function argument : %s %s\n", $1, $2); free($1); free($2);}

if_block        : if_block_cond if_block
                {block_depth--; indent(); printf("IF END \n");}
                | if_block_cond while_block
                {block_depth--; indent(); printf("IF END \n");}
                | if_block_cond statement
                {block_depth--; indent(); printf("IF END \n");}
                | if_block_cond code_block
                {block_depth--; indent(); printf("IF END \n");}
                ;

if_block_cond   : IF LEFTROUNDBRACKET equal_exp RIGHTROUNDBRACKET
                {indent();printf("IF BEGIN \n"); block_depth++;}

while_block     : while_block_cond while_block
                {block_depth--; indent(); printf("WHILE END \n");}
                | while_block_cond if_block
                {block_depth--; indent(); printf("WHILE END \n");}
                | while_block_cond statement
                {block_depth--; indent(); printf("WHILE END \n");}
                | while_block_cond code_block
                {block_depth--; indent(); printf("WHILE END \n");}
                ;

while_block_cond: WHILE LEFTROUNDBRACKET equal_exp RIGHTROUNDBRACKET
                {indent();printf("WHILE BEGIN \n"); block_depth++;}


compare_exp     : add_expression
                {}
                | compare_exp LEFTANGLEBRACKET add_expression
                {indent(); printf("compare operation : <\n");}
                | compare_exp LEFTANGLEBRACKET EQ add_expression
                {indent();printf("compare operation : <=\n");}
                | compare_exp RIGHTANGLEBRACKET add_expression
                {indent(); printf("compare operation : >\n");}
                | compare_exp RIGHTANGLEBRACKET EQ add_expression
                {indent(); printf("compare operation : >=\n");}
                ;

equal_exp       : compare_exp
                {}
                | equal_exp EQ_OP compare_exp
                {indent();printf("compare operation : ==\n");}
                | equal_exp NEQ compare_exp
                {indent();printf("compare operation : !=\n");}
                ;

add_expression  : mul_expression
                {}
                | add_expression PLUS mul_expression
                {indent();printf("operation : +\n");}
                | add_expression MINUS mul_expression
                {indent();printf("operation : -\n");}
                ;

mul_expression  : expression_value
                {}
                | mul_expression MUL expression_value
                {indent();printf("operation : *\n");}
                | mul_expression DIV expression_value
                {indent();printf("operation : /\n");}
                ;

expression_value: const_value
                {}
                | value
                {}
                ;

variable        : ID
                {indent();printf("use variable : %s\n", $1); $$ = $1;}
                | MUL variable
                {indent();printf("-- use pointer's reference\n"); $$ = $2;}
                | REF variable
                {indent();printf("-- use address\n"); $$ = $2;}
                ;

const_value     : NUMBER
                {indent();printf("use constant int : %u\n", $1);}
                | CHAR
                {indent();printf("use constant char : %c\n", $1);}
                | STRING
                {indent();printf("use constant string : %s\n", $1); free($1);}
                ;          

value           : LEFTROUNDBRACKET equal_exp RIGHTROUNDBRACKET
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
  fprintf(stderr,"%s\n", s);
}