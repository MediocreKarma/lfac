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
%token WHILE FOR IF ELSE DO RETURN EVAL TYPEOF
%token CLASS CONST ARRAY FN THIS
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

class_definition: CLASS ID '{' { symbolTable.enterScope($2); } class_members '}' {symbolTable.exitScope();} ;


class_members: class_members class_member SEP
             | // eps
             ;

// trebuie ca intr-o functie sa putem avea... this. Sa putem avea pointerul this. sau gen nu stiu. sa putem folosi keywordul this. Dar nu stiu cum exprim asta..
class_member: decl
            | function_definition
            ;

functions_block: functions_block function_definition
               | // eps
               ;

function_definition: FN TYPE ID '(' list_param ')' '{' {symbolTable.enterScope($3);} statement_list '}' {symbolTable.exitScope();}
                    ;

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
     | CONST TYPE ID ASSIGN expr {}                                                                                     // amc vrem -- bine...
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
          // TREBUIE sa putem avea declarari ca statementuri in functii
          | decl {}
          // id call_list ? -- gen functii. function calls
          | ID '(' call_list ')' {}
          // pareri syntaxa pitonica? daca obligam '{}' atunci nu ne trebuie delimitator
          // -- daca nu ne incurca in alte parti, ok
          // stai unde vrei sa obligi {}?
          | IF expr { /* if type of expression IS NOT bool, then raise semantic error. ne trebuie un %type pentru expr... dar nu stiu momentan ce, si depinde de daca ne cere intr-adevar Doar Expresii Aritmetice Si Booleane. daca nu... it all becomes ASTs*/ }':'  { /* add randomized scope name */} block {/*symbolTable.exitScope();*/}
          | ELSE ':' {} // de ce e else ul asta separat...
          | WHILE expr { /* if type of expression IS NOT bool, then raise semantic error*/ }':' { /* add randomized scope name */} block { /*symbolTable.exitScope();*/}
          | FOR assignment SEP expr {/* if type of expression IS NOT bool, then raise semantic error*/ } SEP assignment ':'  { /* add randomized scope name */} block  { /*symbolTable.exitScope();*/}
          // niste mici conflicte intre do while si while, 
          // uncomment at your own risk
          // | DO ':' block WHILE expr 
          | EVAL '(' expr ')' {/*print ceva ceva*/}
          | TYPEOF '(' expr ')' {}
          ;

assignment: identifier ASSIGN expr {/*check if value of AST is same as... etc*/}
          ;
        

call_list : expr
          | call_list ',' expr
          ;

expr : '(' expr ')'
     | expr PLUS  expr {/* general todo: check if types are The Same, then do the operation and return the needed node*/}
     | expr MINUS expr {}
     | expr  MUL  expr {}
     | expr  DIV  expr {}
     | expr  POW  expr {}
     | expr ANDB  expr {}
     | expr  ORB  expr {}
     | expr  EQ   expr {}
     | expr  NEQ  expr {}
     |      NEGB  expr {}
     | expr  LT   expr {}
     | expr  LEQ  expr {}
     | expr  GT   expr {}
     | expr  GEQ  expr {}
     | INTVAL { std::cout << "Int Literal: " << $1 << std::endl; }
     | FLOATVAL { std::cout << "Float Literal: " << $1 << std::endl; }
     | BOOLVAL { std::cout << "Bool Literal: " << $1 << std::endl; }
     | STRINGVAL { std::cout << "String Literal: " << $1 << std::endl; }
     | CHARVAL { std::cout << "Char Literal: " << $1 << std::endl; }
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