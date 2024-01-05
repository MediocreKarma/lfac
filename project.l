%{
#include <stdio.h>
#include "project.tab.h"

char* preprocessStringLiteral(const char* input) {
    if (input == NULL) {
        return NULL;
    }
    size_t inputLength = 0;
    while (input[inputLength] != '\0') {
        inputLength++;
    }

    char* result = (char*)malloc((2 * inputLength + 2) * sizeof(char));
    if (result == NULL) {
        return NULL;
    }
    size_t resultIndex = 0;

    char prevChar = input[0];
    //                  ca sa ignor quotation marks at beginning 
    for (size_t i = 1; i < inputLength - 1; i++) {
        if (prevChar != '\\') {
            if (input[i] != '\\') {
                result[resultIndex++] = input[i];
            }
            prevChar = input[i];
            continue;
        }
        switch(input[i]) {
            case 'n':
                result[resultIndex++] = '\n';
                break;
            case '0':
                result[resultIndex++] = '\0';
                break;
            case '"':
                result[resultIndex++] = '"';
                break;
            default:
                result[resultIndex++] = prevChar;
                result[resultIndex++] = input[i];
                break;
        }
        prevChar = input[i];
    }
    result[resultIndex] = '\0';
    return result;
}

char preprocessCharLiteral(const char* input) {
    char* processed = preprocessStringLiteral(input);
    if (processed == NULL or strlen(processed) < 1) {
        return '\0';
    }

    char desired = processed[0];
    free(processed);
    return desired;
}

%}
%option noyywrap
%%

"int"|"float"|"char"|"string"|"bool" {yylval.stringValue = strdup(yytext); return TYPE;}
"const" {return CONST;}
"class" {return CLASS;}
"array" {return ARRAY;}
"self" {return SELF;}
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
\"([^\"\n\0]|"\\\""|"\\0"|"\\n")*\" { yylval.stringValue = preprocessStringLiteral(yytext); return STRINGVAL;}
\'([^\n\0]|"\\\""|"\\0"|"\\n")\' {yylval.charValue = preprocessCharLiteral(yytext); return CHARVAL;}
[ \t] ;
\n {yylineno++;}
. {return yytext[0];}

 