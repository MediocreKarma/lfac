%{
#include <iostream>
#include <vector>
#include <string>
#include "types.h"
#include "AST.h"

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
     class AST* astNode;
}
%token CLASSES ENDCLASSES FUNCTIONS ENDFUNCTIONS GLOBALS ENDGLOBALS MAIN
%token SEP ASSIGN INCREMENT DECREMENT
%token WHILE FOR IF ELSE DO RETURN EVAL TYPEOF
%token CLASS CONST ARRAY FN THIS
%token<idValue> TYPE ID
%token<stringValue> STRINGVAL
%token<intValue> INTVAL
%token<floatValue> FLOATVAL
%token<boolValue> BOOLVAL
%token<charValue> CHARVAL
%type<astNode> expr

%left PLUS MINUS DIV MUL ANDB ORB POW
%left POSTINCR POSTDECR
%nonassoc LEQ GEQ LT GT EQ NEQ
%right NEGB
%right PREINCR PREDECR

%start start_program
%%

start_program : progr { std::cout << "The code syntax is correct.\n";}

progr: classes_section globals_section functions_section MAIN block
     ;

classes_section: CLASSES classes_block ENDCLASSES
               | /* epsilon */
               ;
          
globals_section: GLOBALS globals_block ENDGLOBALS
               | /* epsilon */
               ;

functions_section: FUNCTIONS functions_block ENDFUNCTIONS
                 | /* epsilon */
                 ;

classes_block: classes_block class_definition
             | /* epsilon */
             ;

class_definition: CLASS ID '{' { symbolTable.enterScope($2); } class_members '}' {symbolTable.exitScope();} ;


class_members: class_member SEP class_members
             | /* epsilon */
             ;

// trebuie ca intr-o functie sa putem avea... this. Sa putem avea pointerul this. sau gen nu stiu. sa putem folosi keywordul this. Dar nu stiu cum exprim asta...

class_member: decl
            | function_definition
            ;

functions_block: function_definition functions_block
               | /* epsilon */
               ;

function_definition: FN TYPE ID '(' list_param ')' '{' {symbolTable.enterScope($3);} statement_list '}' {symbolTable.exitScope();}
                    | FN CLASS ID ID'(' list_param ')' '{' {symbolTable.enterScope($4);} statement_list '}' {symbolTable.exitScope();}
                    ;

globals_block: decl SEP globals_block
             | /* epsilon */
	        ;
      

// useless ? -- poate un block ar putea sa fie un statement-list cu acolade in jurul lui
block : '{' statement_list '}' 
     ;

//what can be assigned to?
identifier: ID
          | ID '.' ID {/*class member*/}
          | ID '[' INTVAL ']' {/* array member*/}
          ;

decl : TYPE ID { 
          if(!symbolTable.contains($2)) {
               symbolTable.add(SymbolData($2, TypeNms::strToType($1), SymbolData::Variable));
          }
          else {
               yyerror("Symbol already exists!");
          }
     }
     | ARRAY TYPE ID '[' INTVAL ']' {
     }
     | CONST TYPE ID ASSIGN expr {
          if(!symbolTable.contains($3)) {
               symbolTable.add(SymbolData($3, TypeNms::strToType($2), SymbolData::Constant));
          }
          else {
               yyerror("Symbol already exists!");
          }
     }                                                                                                                          // amc vrem -- bine...
     | CLASS ID ID  /*ca sa trebuiasca sa spunem "class myClass x;" cand declaram o instanta a unei clase. presupunem ca nu vrem constructori la clase...*/ {}
     | FN TYPE ID '(' list_param ')' /* astea cred ca s function declarations... nuj exact daca sa scriu asa sau daca sa trantesc function_definition si sa iau aici informatiile din $$ */ {}
     | FN TYPE ID '(' ')' {}
     ;

list_param :  param
            | param ','  list_param 
            ;
            
param : TYPE ID
      | CLASS ID ID
      ; 
     

statement_list : statement SEP
               | statement SEP statement_list
               ;

statement : assignment

          // trebuie sa putem avea declarari ca statementuri in functii ca sa putem avea functii in diverse scope-uri
          | decl {}

          // functii. function calls
          | ID '(' call_list ')' {}

          | IF expr { /* -- if type of expression IS NOT bool, then raise semantic error. ne trebuie un %type pentru expr... dar nu stiu momentan ce, si depinde de daca ne cere intr-adevar Doar Expresii Aritmetice Si Booleane. daca nu... it all becomes ASTs*/ }':'  { /* add randomized scope name */} block {/*symbolTable.exitScope();*/} ELSE ':'  block {} 

          | WHILE expr { /* -- if type of expression IS NOT bool, then raise semantic error*/ }':' { /* add randomized scope name */} block { /*symbolTable.exitScope();*/}

          | FOR assignment SEP expr {/* -- if type of expression IS NOT bool, then raise semantic error*/ } SEP assignment ':'  { /* add randomized scope name */} block { /*symbolTable.exitScope();*/}

          | DO ':' block WHILE expr
          | EVAL '(' expr ')' {std::cout << "In Eval\n";}
          | TYPEOF '(' expr ')' {std::cout << "In TypeOf\n";}
          ;

assignment: identifier ASSIGN expr {/*check if value of AST is same as... etc*/}
          ;
        

// sper sa nu fie ceva gresit aici (vreau sa pot avea ori f(a), ori f(a, b, c), ori f()... )
call_list : expr ',' {/**/} call_list
          | expr
          | /* epsilon */
          ;

// e ok sa fie o singura expresie pentru toate tipurile de expresii?
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
     | INTVAL { std::cout << "Int Literal: " << $1 << std::endl; $$ = new AST(); }
     | FLOATVAL { std::cout << "Float Literal: " << $1 << std::endl; }
     | BOOLVAL { std::cout << "Bool Literal: " << $1 << std::endl; }
     | STRINGVAL { std::cout << "String Literal: " << $1 << std::endl; }
     | CHARVAL { std::cout << "Char Literal: " << $1 << std::endl;  }
     | identifier { /* add new AST no matter what, any type is valid */}
     | ID '(' call_list ')' {}  /* function calls */
     ;

// cred ca inclusiv literalilor ar trebui sa le dam SymbolData
// si sa specificam 
/* literal : INTVAL | CHARVAL | BOOLVAL | STRINGVAL | FLOATVAL;

value : literal | identifier;

initializer_list : '{' identifier_list '}'

identifier_list : value |  value SEP identifier_list */

%%
void yyerror(const char * s){
     printf("Error: %s at line: %d\n",s,yylineno);
     exit(1);
}

int main(int argc, char** argv){
     yyin=fopen(argv[1],"r");
     yyparse();
     std::cout << "Basetype, non-const, non-array, non-class variables:" << std::endl;
     symbolTable.print(std::cout);
} 