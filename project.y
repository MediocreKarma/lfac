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
     class AST* astNode; // de ce AST* in union?
}
%token CLASSES ENDCLASSES FUNCTIONS ENDFUNCTIONS GLOBALS ENDGLOBALS MAIN
%token SEP ASSIGN INCREMENT DECREMENT DEF DECL
%token WHILE FOR IF ELSE DO RETURN EVAL TYPEOF
%token CLASS CONST ARRAY FN SELF
%token<idValue> TYPE ID
%token<stringValue> STRINGVAL
%token<intValue> INTVAL
%token<floatValue> FLOATVAL
%token<boolValue> BOOLVAL
%token<charValue> CHARVAL
%type<astNode> expr

%left PLUS MINUS DIV MUL ANDB ORB POW
%nonassoc LEQ GEQ LT GT EQ NEQ
%right NEGB

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

class_definition: CLASS ID '{' { symbolTable.enterScope(""); } DECL '{' class_members '}' DEF '{' class_methods_def '}' '}' {symbolTable.exitScope();};

class_members: decl SEP class_members
             | /* epsilon */
             ;

// no sep needed when blocks are used
class_methods_def   : function_definition class_methods_def {}
                    | /* epsilon */

// trebuie ca intr-o functie sa putem avea... this. Sa putem avea pointerul this. sau gen nu stiu. sa putem folosi keywordul this. Dar nu stiu cum exprim asta...

functions_block: function_definition functions_block
               | /* epsilon */
               ;

function_declaration: FN TYPE ID '(' fn_param ')' 
                    | FN CLASS ID ID '(' fn_param ')'
                    ;

function_definition : function_declaration block
                    ;

globals_block: decl SEP globals_block
             | /* epsilon */
	        ;
      

// useless ? -- poate un block ar putea sa fie un statement-list cu acolade in jurul lui
block : '{' {symbolTable.enterScope(""); } statement_list '}' { symbolTable.exitScope();} 
     ;

//what can be assigned to?
identifier: ID
          | ID '.' ID {/*class member*/}
          | ID '[' INTVAL ']' {/* array member*/}
          | SELF '.' ID {/*class member*/}
          ;

decl : TYPE ID { 
          if(!symbolTable.contains($2)) {
               symbolTable.add(SymbolData($2, TypeNms::strToType($1), SymbolData::Variable));
          }
          else {
               yyerror("Symbol already exists!");
          }
     }
     | TYPE ID ASSIGN expr
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
     | function_declaration /* astea cred ca s function declarations... nuj exact daca sa scriu asa sau daca sa trantesc function_definition si sa iau aici informatiile din $$ */ {}
     ;

fn_param  : list_param 
          | /* epsilon */ 
          ;

list_param     : param {}
               | param ',' list_param {}
               ;
            
param     : TYPE ID { std::cout << $1 << ' ' << $2 << '\n';}
          | CLASS ID ID
          ; 
     

statement_list : stmt statement_list
               | /* epsilon */
               ;

stmt : sep_stmt SEP
     | non_sep_stmt
     ;

sep_stmt       : assignment {}
               | RETURN expr {/*nimic*/}
               // trebuie sa putem avea declarari ca statementuri in functii ca sa putem avea chetii in diverse scope-uri
               | decl {}
               | EVAL '(' expr ')' {/* get value of expr as string, cout*/}
               | TYPEOF '(' expr ')' {/*get strToType*/}
               | DO ':' {symbolTable.enterScope(SymbolTable::RandomizedScopes::DoWhile);} block WHILE {symbolTable.exitScope();} expr {/*check if expr bool*/} 
               | expr { /*kinda just ignore*/}

non_sep_stmt   : if_expr ELSE ':' block {} 
               | if_expr {}
               | WHILE expr { /* check if expr bool */ }':' {symbolTable.enterScope(SymbolTable::RandomizedScopes::While);} block { symbolTable.exitScope();}
               | FOR {symbolTable.enterScope(SymbolTable::RandomizedScopes::For);} assignment {} SEP expr {/*check if expr bool */ } SEP assignment ':'  {} block { symbolTable.exitScope();}
               | block {}

if_expr   : IF expr { /* verify expr */ } ':' { symbolTable.enterScope(SymbolTable::RandomizedScopes::If);} block { symbolTable.exitScope();}

assignment: identifier ASSIGN expr {/*check if type of AST is type of identifier, also check if identifier is basetype (can't have other types of exprs... cred)*/}
          ;
        

// o sa perm compilarea la f(1,2,3,)
call_list : expr ',' {/**/} call_list
          | expr
          | /* epsilon */
          ;

// e ok sa fie o singura expresie pentru toate tipurile de expresii?
expr : '(' expr ')' {}
     | expr PLUS  expr {}
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
     | MINUS expr {}
     | INCREMENT identifier {}
     | DECREMENT identifier {}
     | identifier INCREMENT {}
     | identifier DECREMENT {}
     | INTVAL {  std::cout << "Int Literal: " << $1 << std::endl;}
     | FLOATVAL { std::cout << "Float Literal: " << $1 << std::endl;} 
     | BOOLVAL { std::cout << "Bool Literal: " << $1 << std::endl; }
     | STRINGVAL { std::cout << "String Literal: " << $1 << std::endl; }
     | CHARVAL { std::cout << "Char Literal: " << $1 << std::endl;}
     | identifier { /* add AST IF!!! identifier is a class member or an array lookup or a simple value. Check in the AST */}
     | ID '(' call_list ')' {}  /* function calls. check if ID has base-type, if not then don't allow it. */
     ;

// cred ca inclusiv literalilor ar trebui sa le dam SymbolData
// si sa specificam 
/* literal : INTVAL | CHARVAL | BOOLVAL | STRINGVAL | FLOATVAL;

value : literal | identifier;

initializer_list : '{' identifier_list '}'

identifier_list : value |  value SEP identifier_list */

%%
void yyerror(const char * s) {
     printf("Error: %s at line: %d\n",s,yylineno);
     exit(1);
}

int main(int argc, char** argv) {
     yyin=fopen(argv[1],"r");
     yyparse();
     std::cout << "Basetype, non-const, non-array, non-class variables:" << std::endl;
     symbolTable.print(std::cout);
} 