%{
#include <iostream>
#include <vector>
#include "AST.h"
#include "IdList.h"
extern FILE* yyin;
extern char* yytext;
extern int yylineno;
extern int yylex();
void yyerror(const char * s);
class IdList ids;
%}
%union {
     char* idValue;
     char* stringValue;
     int64_t intValue;
     double floatValue;
     bool boolValue;
     char charValue;
}
%token CLASSES ENDCLASSES FUNCTIONS ENDFUNCTIONS GLOBALS ENDGLOBALS MAIN ENDMAIN
%token SEP ASSIGN INCREMENT DECREMENT
%token WHILE FOR IF ELSE DO RETURN
%token CLASS CONST ARRAY FN
%token<idValue> TYPE ID
%token<stringValue> STRINGVAL
%token<intValue> INTVAL
%token<floatValue> FLOATVAL
%token<boolValue> BOOLVAL
%token<charValue> CHARVAL
%type<boolValue> bool_expr

%left PLUS MINUS DIV MUL ANDB ORB POW
%left POSTINCR POSTDECR
%nonassoc LEQ GEQ LT GT EQ NEQ
%right NEGB
%right PREINCR PREDECR

%start start_program
%%

start_program : progr { cout << "The code syntax is correct.\n";}

progr: classes_section globals_section functions_section MAIN block ENDMAIN
     ;

classes_section: CLASSES classes_block ENDCLASSES
               | // eps
               ;
          
globals_section: GLOBALS globals_block ENDGLOBALS
               | // eps
               ;

functions_section: FUNCTIONS functions_block ENDFUNCTIONS
                 | // eps
                 ;

classes_block: classes_block class_definition
             | // eps
             ;

class_definition: CLASS ID '{' {/*enter class scope: toate declararile si asa se vor face in contextul clasei mele*/} class_members {/*exit class scope*/} '}';

class_members: class_members class_member SEP
             | // eps
             ;

class_member: decl
            | function_definition
            ;

functions_block: functions_block function_definition
               | // eps
               ;

function_definition: FN TYPE ID '(' list_param ')' '{' statement_list '}' 

globals_block: decl SEP globals_block
             | // eps
	        ;
      

block : statement_list 
     ;

decl : TYPE ID { if(!ids.existsVar($2)) {
                    ids.addVar($1,$2);
               }
          }
     | CONST TYPE ID ASSIGN arith_expr {}                                                                                     // amc vrem
     | CLASS ID ID  /*ca sa trebuiasca sa spunem "class myClass x;" cand declaram o instanta a unei clase. presupunem ca nu vrem constructori la clase...*/ {}
     | FN TYPE ID '(' list_param ')' /* astea cred ca s function declarations... nuj exact daca sa scriu asa sau daca sa trantesc function_definition si sa iau aici informatiile din $$ */ {}
     | FN TYPE ID '(' ')' {}
     ;

list_param : param
            | list_param ','  param 
            ;
            
param : TYPE ID
      | CLASS ID ID
      ; 
     

statement_list :  statement SEP
               | statement SEP statement_list
               ;

statement: assignment
         | ID '(' call_list ')'
         | WHILE '(' ID ')' '{' statement '}'
         ;

assignment: ID ASSIGN expr {}
          ;
        

call_list : arith_expr
          | call_list ',' arith_expr
          ;

expr : bool_expr
     | arith_expr
     | str_expr 
     ;

arith_expr: '(' arith_expr ')'
          | arith_expr PLUS arith_expr
          | arith_expr MINUS arith_expr
          | arith_expr MUL arith_expr
          | arith_expr DIV arith_expr
          | arith_expr POW arith_expr
          /*plus absolut toate celelalte valori. function calls, variabile, etc*/
          | INTVAL {}
          | FLOATVAL {}
          | CHARVAL {}
          | INCREMENT ID
          | ID INCREMENT
          | DECREMENT ID
          | ID DECREMENT
          | ID {} /* variabila efectiva*/
          | ID '(' call_list ')' {}  /* function calls */
          | ID '(' ')' {}
          /* si pe aici trb adaugate alea cu operatorii, + - < > etc*/
          ;

str_expr  : '(' str_expr ')'
          | str_expr PLUS str_expr
          | STRINGVAL {}
          | ID {}
          ;

bool_expr : '(' bool_expr ')' { $$ = $2; }
          | bool_expr ANDB bool_expr
          | bool_expr ORB  bool_expr
          | bool_expr EQ   bool_expr
          | bool_expr NEQ  bool_expr
          |           NEGB bool_expr
          | arith_expr LT  arith_expr
          | arith_expr LEQ arith_expr
          | arith_expr GT  arith_expr
          | arith_expr GEQ arith_expr
          | arith_expr EQ  arith_expr
          | arith_expr NEQ arith_expr
          | arith_expr
          | str_expr LT  str_expr
          | str_expr LEQ str_expr
          | str_expr GT  str_expr
          | str_expr GEQ str_expr
          | str_expr EQ  str_expr
          | str_expr NEQ str_expr
          | str_expr
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