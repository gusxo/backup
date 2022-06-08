%{
#include<stdio.h>
%}

%%
[a-zA-Z_][a-zA-Z_0-9]*  {printf("%s : symbol\n", yytext);}
"("   {printf("%s : left bracket\n", yytext);}
")"   {printf("%s : right bracket\n", yytext);}
"+"   {printf("%s : plus\n", yytext);}
"*"   {printf("%s : multi\n", yytext);}
\n  {return 0;}
" " {}
.   {printf("%s : unknown\n", yytext);}
%%
int main(){
    yylex();
    return 0;
}