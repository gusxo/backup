%{
#include <stdio.h>
#include <string.h>
#include "y.tab.h"
int yyerror(char* s);
%}

%%
[a-zA-Z_][a-zA-Z_0-9]*  {yylval.str = strdup(yytext); return ID;}
"("   {return LEFTBRACKET;}
")"   {return RIGHTBRACKET;}
"+"   {return PLUS;}
"*"   {return MUL;}
\n  {return 0;}
[ \t]+ /* ignore whitespace */
.   {yyerror("unknown token");}
%%