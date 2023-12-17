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
%token MAIN END ASSIGN WHILE FOR IF DEFINITIONS GLOBALS FUNCTIONS CLASS
%token<stringValue> TYPE ID
%token<intValue> INTVAL
%token<floatValue> FLOATVAL
%token<boolValue> BOOLVAL

%left PLUS MINUS DIV MUL ANDB ORB NEGB
%nonassoc LEQ GEQ LT GT EQ NEQ


%start progr
%%

/*incomplet ca trb adaugate si alea de class definitions si asa*/
progr:  GLOBALS globals_block MAIN block END {printf("The programme is correct!\n");}
     ;

definitions_block : /*placeholder -- aici trb puse definiri de clase*/ %empty;

functions_block : /*placeholder -- function definitions*/ %empty;

globals_block :  decl ';'          
	      |  globals_block decl ';'   
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
           | CLASS ID ID  /*ca sa trebuiasca sa spunem "class myClass x;" cand declaram o instanta a unei clase. presupunem ca nu vrem constructori la clase...*/ {}
           | TYPE ID '(' list_param ')' /* astea cred ca s function declarations */ {}
           | TYPE ID '(' ')' {}
           
           ;

list_param : param
            | list_param ','  param 
            ;
            
param : TYPE ID 
      ; 
     

statement_list :  statement ';' 
     | statement_list statement ';'
     ;

assignment : ID ASSIGN ID 
           | ID ASSIGN INTVAL
           ;

statement: assignment
         | ID '(' call_list ')'
         | WHILE '(' ID ')' '{' statement '}'
         ;
        

/*trb modificat sa poata lua orice... etc*/
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