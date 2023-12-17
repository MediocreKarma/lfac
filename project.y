%{
#include <iostream>
#include <vector>
#include "IdList.h"
using namespace std;
extern FILE* yyin;
extern char* yytext;
extern int yylineno;
extern int yylex();
void yyerror(const char * s);
class IdList ids;
%}
%union {
     const char* stringValue;
     long long intValue;
     double floatValue;
     bool boolValue;
     char charValue;
}
%token SEP MAIN END ASSIGN WHILE FOR IF DEFINITIONS GLOBALS FUNCTIONS CLASS CLASSES CONST
%token<stringValue> TYPE ID
%token<intValue> INTVAL
%token<floatValue> FLOATVAL
%token<boolValue> BOOLVAL

%left PLUS MINUS DIV MUL ANDB ORB NEGB
%nonassoc LEQ GEQ LT GT EQ NEQ

%start progr
%%

/*incomplet ca trb adaugate si alea de class definitions si asa*/
progr: CLASSES classes_block GLOBALS globals_block MAIN block END {printf("The programme is correct!\n");}
     ;

definitions_block : %empty;

classes_block : class_definition
              | classes_block class_definition
              ;

class_definition: CLASS ID '{' {/*enter class scope: toate declararile si asa se vor face in contextul clasei mele*/} class_members {/*exit class scope*/} '}';

class_members: class_member SEP
               | class_members class_member SEP

class_member : decl
               | function_definition
               ;

functions_block :  function_definition SEP
                | functions_block function_definition SEP;

function_definition: TYPE ID '(' list_param ')' '{' statement_list '}' 

globals_block :  decl SEP
	      |  globals_block decl SEP
	      ;
      

block : statement_list  
     ;

/* cand ne dam seama ce facem cu tipurile, pe aici trebe construit AST-ul */
expression : INTVAL {}
           | BOOLVAL {}
           | FLOATVAL {}
           /*plus absolut toate celelalte valori. function calls, variabile, etc*/
           | ID {} /* variabila efectiva*/
           | ID '(' call_list ')' {}  /* function calls */
           | ID '(' ')' {}
           /* si pe aici trb adaugate alea cu operatorii, + - < > etc*/
           ;

decl       :  TYPE ID { if(!ids.existsVar($2)) {
                          ids.addVar($1,$2);
                     }
                    }
           | CONST ID ASSIGN expression {}
           | CLASS ID ID  /*ca sa trebuiasca sa spunem "class myClass x;" cand declaram o instanta a unei clase. presupunem ca nu vrem constructori la clase...*/ {}
           | TYPE ID '(' list_param ')' /* astea cred ca s function declarations */ {}
           | TYPE ID '(' ')' {}
           
           ;

list_param : param
            | list_param ','  param 
            ;
            
param : TYPE ID
      | CLASS ID ID
      ; 
     

statement_list :  statement SEP 
     | statement_list statement SEP
     ;

assignment : ID ASSIGN expression {}
           ;

statement: assignment
         | ID '(' call_list ')'
         | WHILE '(' ID ')' '{' statement '}'
         ;
        

call_list : expression
           | call_list ',' expression
           ;

%%
void yyerror(const char * s){
printf("error: %s at line: %d\n",s,yylineno);
}

int main(int argc, char** argv){
     yyin=fopen(argv[1],"r");
     yyparse();
     cout << "Variables:" <<endl;
     ids.printVars();
    
} 