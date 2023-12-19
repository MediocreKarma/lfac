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

class SymbolTable symbolTable;

class BoolClass;
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
      

// useless ?
block : statement_list 
     ;

//what can be assigned to?
identifier: ID
          | ID '.' ID {/*class member*/}
          | ID '[' INTVAL ']' {/* array member*/}
          ;

decl : TYPE ID { 
          if(!symbolTable.contains($2)) {
               symbolTable.add(SymbolData($2, typeFromString($1), SymbolData::Variable));
          }
     }
     | CONST TYPE ID ASSIGN expr {}                                                                                     // amc vrem
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
     

statement_list : statement SEP
               | statement SEP statement_list
               ;

statement : assignment
          // id call_list ?
          | ID '(' call_list ')'
          // pareri syntaxa pitonica? daca obligam '{}' atunci nu ne trebuie delimitator
          // -- daca nu ne incurca in alte parti, ok
          | IF expr ':' block
          | ELSE ':' 
          | WHILE expr ':' block
          | FOR assignment SEP expr SEP assignment ':' block
          // niste mici conflicte intre do while si while, 
          // uncomment at your own risk
          // | DO ':' block WHILE bool_expr 
          ;

assignment: identifier ASSIGN expr {/*check if value of AST is same as... etc*/}
          ;
        

call_list : expr
          | call_list ',' expr
          ;

//value of arith_expr should probably be an AST
expr : '(' expr ')'
     | expr PLUS  expr /* trebuie sa se verifice daca sunt ambele inturi sau ambele float-uri. chestii de genul*/
     | expr MINUS expr
     | expr  MUL  expr
     | expr  DIV  expr
     | expr  POW  expr
     | expr ANDB  expr
     | expr  ORB  expr
     | expr  EQ   expr {/*check if types of ASTs are exactly the same*/}
     | expr  NEQ  expr
     |      NEGB  expr
     // BOOLVAL 
     | expr  LT   expr {/* in acest caz stim efectiv ca astea s de tipuri ok, ca s-a verificat deja la nivel de arith_expr*/}
     | expr  LEQ  expr
     | expr  GT   expr
     | expr  GEQ  expr
     | INTVAL { std::cout << "Int: " << $1 << std::endl; }
     | FLOATVAL { std::cout << "Float: " << $1 << std::endl; }
     | BOOLVAL { std::cout << "Bool: " << $1 << std::endl; }
     | STRINGVAL { std::cout << "String: " << $1 << std::endl; }
     | CHARVAL { std::cout << "Char: " << $1 << std::endl; }
     | identifier {/* add new AST no matter what, any type is valid */}
     | ID '(' call_list ')' {}  /* function calls */
     | ID '(' ')' {}
     ;

%%
void yyerror(const char * s){
     printf("Error: %s at line: %d\n",s,yylineno);
}

int main(int argc, char** argv){
     yyin=fopen(argv[1],"r");
     yyparse();
     std::cout << "Variables:" << std::endl;
     symbolTable.print(std::cout);
} 