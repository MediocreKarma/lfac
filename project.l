%{
#include <stdio.h>
#include "project.tab.h"
%}
%option noyywrap
%%
"int"|"float"|"char"|"string"|"bool" {yylval.string = strdup(yytext);return BASE_TYPE;}
"BEGIN" {return BGIN;}
"END" {return END;}
[a-zA-Z][a-zA-Z0-9]* {yylval.string = strdup(yytext); return ID;}
":=" {return ASSIGN;}
[1-9][0-9]* {return NR;}
"while" {return WHILE;}
"for" {return FOR;}
"if" {return IF;}
[ \t] ;
\n {yylineno++;}
. {return yytext[0];}

 