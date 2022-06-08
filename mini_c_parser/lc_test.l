%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%%
int {printf("%s : INTTYPE\n", yytext); }
char {printf("%s : CHARTYPE\n", yytext); ;}
if  {printf("%s : IF\n", yytext); ;}
while   {printf("%s : WHILE\n", yytext); ;}
include   {printf("%s : INCLUDE\n", yytext); ;}
return  {printf("%s : RETURN\n", yytext);}
'.'     {printf("%s : CHAR\n", yytext);}
\<.*>  {
    unsigned int len = strlen(yytext); 
    char* str = malloc(len-1); 
    strncpy(str, yytext+1, len-2);
    str[len-2] = '\0';
    printf("%s : include path\n", str);
    }
\".*\"  {
    unsigned int len = strlen(yytext); 
    char* str = malloc(len-1); 
    strncpy(str, yytext+1, len-2);
    str[len-2] = '\0';
    printf("%s : string\n", str);
    }
[a-zA-Z_][a-zA-Z_0-9]*  {printf("%s : ID\n", yytext);}
([0-9])|([1-9][0-9]+) {printf("%d : num\n", atoi(yytext));}
;   {printf("%s : SEMICOLON\n", yytext); ;}
,   {printf("%s : COMMA\n", yytext); ;}
"("      {printf("%s : LEFTROUNDBRACKET\n", yytext); ;}
")"      {printf("%s : RIGHTROUNDBRACKET\n", yytext); }
"{"      {printf("%s : LEFTCRULYBRACKET\n", yytext); }
"}"      {printf("%s : RIGHTCRULYBRACKET\n", yytext);}
"["      {printf("%s : LEFTBOXBRACKET\n", yytext);}
"]"      {printf("%s : RIGHTBOXBRACKET\n", yytext); }
"<"      {printf("%s : LEFTANGLEBRACKET\n", yytext); }
">"      {printf("%s : RIGHTANGLEBRACKET\n", yytext); }
"+"       {printf("%s : PLUS\n", yytext); }
"-"       {printf("%s : MINUS\n", yytext); }
"/"       {printf("%s : DIV\n", yytext);}
"*"       {printf("%s : MUL\n", yytext);}
"="       {printf("%s : EQ\n", yytext);}
\n      {printf("NEWLINE\n");}
[ \t]+  /* ignore whitespace */
.       {printf("%s : unknown token\n", yytext);}
%%

int main(int argc, char** argv){
  // open a file handle to a particular file:
  FILE *myfile = fopen(argv[1], "r");
  // make sure it's valid:
  if (!myfile) {
    printf("I can't open the file!");
    return -1;
  }
  // set lex to read from it instead of defaulting to STDIN:
  yyin = myfile;
  
  // lex through the input:
  while(yylex());
  fclose(myfile);
}