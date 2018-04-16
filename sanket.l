%{
#include "y.tab.h"
#include<stdio.h>
extern int yylval;
%}
%%

[0-9]+\.[0-9]+ {yylval=atoi(yytext); return NUMBER;}
. {return yytext[0];}
[\t]+ ;
\n {return 0;} 
%%


/*command:
yacc -d sanket.y && lex sanket.l && gcc lex.yy.c y.tab.c -ll && ./a.out

6+8
sum ::14
*/
