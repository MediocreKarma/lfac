%{
#include <stdio.h>
#include "project.tab.h"
%}
%option noyywrap
%%
"int"|"float"|"char"|"string"|"bool" {yylval.stringValue = strdup(yytext);return TYPE;}
"class" {return CLASS;}
"DEFINITIONS" {return DEFINITIONS;} 
"GLOBALS" {return GLOBALS;}
"FUNCTIONS" {return FUNCTIONS;}
"MAIN" {return MAIN;}
"END" {return END;}
"true" {yylval.boolValue = true; return BOOLVAL;}
"false" {yylval.boolValue = false; return BOOLVAL;}
":=" {return ASSIGN;}
"while" {return WHILE;}
"for" {return FOR;}
"if" {return IF;}
"+" {return PLUS;}
"-" {return MINUS;}
"/" {return DIV;}
"*" {return MUL;}
"<=" {return LEQ;}
">=" {return GEQ;}
"<" {return LT;}
">" {return GT;}
"!=" {return NEQ;}
"==" {return EQ;}
"&&" {return ANDB;}
"||" {return ORB;}
"!" {return NEGB;}
[a-zA-Z_][_a-zA-Z0-9]* {yylval.stringValue = strdup(yytext); return ID;}
[1-9][0-9]*\.[0-9]* {yylval.floatValue = atof(yytext); return FLOATVAL;}
[1-9][0-9]* {yylval.intValue = atol(yytext); return INTVAL;}
[ \t] ;
\n {yylineno++;}
. {return yytext[0];}

 