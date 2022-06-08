%{
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
int yylex(void);
int yyerror(char* s);
%}
%union{
        char* str;
        int num;
}

%token <str> ID LEFTBRACKET RIGHTBRACKET
%left PLUS MUL

%%

expr:   expr PLUS term
        {printf("PLUS\n");}
        | term
        {}
        ;
term:   term MUL factor
        {printf("MUL\n");}
        | factor
        {}
        ;
factor: LEFTBRACKET expr RIGHTBRACKET
        {}
        | ID
        {printf("ID = %s\n", $1);}
        ;

%%
int main(){
        yyparse();
}
int yyerror(char *s)
{
  fprintf(stderr, "error: %s\n", s);
}