%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"
int yyerror(char* s);
%}

%%
int {return INTTYPE;}
char {return CHARTYPE;}
if  {return IF;}
while   {return WHILE;}
include {return INCLUDE;}
return  {return RETURN;}
'.'     {yylval.num = (unsigned int)yytext[1]; return CHAR;}
\<.*>  {
    unsigned int len = strlen(yytext); 
    char* str = malloc(len-1); 
    strncpy(str, yytext+1, len-2);
    str[len-2] = '\0';
    yylval.str = str; return INCLUDEPATH;}
\".*\"  {
    unsigned int len = strlen(yytext); 
    char* str = malloc(len-1); 
    strncpy(str, yytext+1, len-2);
    str[len-2] = '\0';
    yylval.str = str; return STRING;}
[a-zA-Z_][a-zA-Z_0-9]*  {yylval.str = strdup(yytext); return ID;}
[1-9][0-9]* {yylval.num = atoi(yytext); return NUMBER;}
;   {return SEMICOLON;}
,   {return COMMA;}
"("      {return LEFTROUNDBRACKET;}
")"      {return RIGHTROUNDBRACKET;}
"{"      {return LEFTCRULYBRACKET;}
"}"      {return RIGHTCRULYBRACKET;}
"["      {return LEFTBOXBRACKET;}
"]"      {return RIGHTBOXBRACKET;}
"<"      {return LEFTANGLEBRACKET;}
">"      {return RIGHTANGLEBRACKET;}
"+"       {return PLUS;}
"-"       {return MINUS;}
"/"       {return DIV;}
"*"       {return MUL;}
"="       {return EQ;}
"#"       {return SHARP;}
[ \t\n\r\f]+  /* ignore whitespace */
.       {yyerror("unknown token");}
%%