%{
#include <stdio.h>
#include "Utils.h"
#include "project.tab.h"

%}
%option noyywrap
%%

"int"|"float"|"char"|"string"|"bool" {yylval.stringValue = strdup(yytext); return TYPE;}
"const" {return CONST;}
"class" {return CLASS;}
"array" {return ARRAY;}
"self" {return SELF;}
"members" {return MEMBERS;}
"fn" {return FN;}
"decl" {return DECL;}
"def" {return DEF;}

"CLASSES" {return CLASSES;}
"ENDCLASSES" {return ENDCLASSES;}
"GLOBALS" {return GLOBALS;}
"ENDGLOBALS" {return ENDGLOBALS;}
"FUNCTIONS" {return FUNCTIONS;}
"ENDFUNCTIONS" {return ENDFUNCTIONS;}
"fn int main()" {return MAIN;}

"Eval" {return EVAL;}
"TypeOf" {return TYPEOF;}

":=" {return ASSIGN;}
(!!)+ {return SEP; /* sa putem avea cate semne de exclamare vrem */}

"if" {return IF;}
"else" {return ELSE;}
"while" {return WHILE;}
"do" {return DO;}
"for" {return FOR;}
"return" {return RETURN;}

"+" {return PLUS;}
"-" {return MINUS;}
"/" {return DIV;}
"*" {return MUL;}
"**" {return POW;}
"++" {return INCREMENT;}
"--" {return DECREMENT;}

"<=" {return LEQ;}
">=" {return GEQ;}
"<" {return LT;}
">" {return GT;}
"!=" {return NEQ;}
"==" {return EQ;}
"&&" {return ANDB;}
"||" {return ORB;}
"!" {return NEGB;}

"true" {yylval.boolValue = true; return BOOLVAL;}
"false" {yylval.boolValue = false; return BOOLVAL;}
[a-zA-Z_][_a-zA-Z0-9]* {yylval.stringValue = strdup(yytext); return ID;}
[1-9][0-9]*\.[0-9]* {yylval.floatValue = atof(yytext); return FLOATVAL;}
[1-9][0-9]* {yylval.intValue = atol(yytext); return INTVAL;}
\"([^\"\n\0\t\r\\\a\b\f\v]|"\\\""|"\\0"|"\\n"|"\\t"|"\\r"|"\\'"|"\\\\"|"\\a"|"\\b"|"\\f"|"\\v")*\" { yylval.stringValue = Utils::preprocessStringLiteral(yytext); return STRINGVAL;}
\'([^\n\0\t\r\\\a\b\f\v]|"\\\""|"\\0"|"\\n"|"\\t"|"\\r"|"\\'"|"\\\\"|"\\a"|"\\b"|"\\f"|"\\v")\' {yylval.charValue = Utils::preprocessCharLiteral(yytext); return CHARVAL;}
[ \t] ;
\n {yylineno++;}
. {return yytext[0];}

 