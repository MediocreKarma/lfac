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

struct SymbolList {
     SymbolList* next;
     SymbolData* symbol;
};

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
     struct SymbolList* list;
     class SymbolData* symbolValue;
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
%type<list> list_param fn_param
%type<symbolValue> function_declaration decl decl_only decl_assign

%left PLUS MINUS DIV MUL ANDB ORB POW
%nonassoc LEQ GEQ LT GT EQ NEQ
%right NEGB

%start start_program
%%

start_program : progr { std::cout << "The code syntax is correct.\n";}

                                                       // trebe facuta adaugarea functiei main in symboltable
progr: classes_section globals_section functions_section MAIN  '{' {symbolTable.add("main", TypeNms::strToType("int"), SymbolData::Function); symbolTable.enterScope("main"); } statement_list '}' { symbolTable.exitScope();}
     ;

classes_section: CLASSES classes_block ENDCLASSES
               | /* epsilon */
               ;
          
globals_section: GLOBALS globals_block ENDGLOBALS
               | /* epsilon */
               ;

functions_section   : FUNCTIONS DECL '{' functions_decl '}' DEF '{' functions_def '}'  ENDFUNCTIONS
                    | /* epsilon */
                    ;

functions_decl : function_declaration SEP functions_decl {}
               | /* done */
               ;

functions_def  : function_definition functions_def {}
               | /* done */
               ;

classes_block: classes_block class_definition
             | /* epsilon */
             ;

class_definition: CLASS ID '{' { symbolTable.enterScope($2); } DECL '{' class_members '}' DEF '{' class_methods_def '}' '}' {/*add class to defined class ids??*/ symbolTable.exitScope();};

class_members: decl SEP class_members
             | /* epsilon */
             ;

// no sep needed when blocks are used
class_methods_def   : function_definition class_methods_def {}
                    | /* epsilon */

function_declaration: FN TYPE ID {symbolTable.enterScope($3); } '(' fn_param ')' {
                         // all fn params were added, succesfully, now add them to the functions data and remove them
                         SymbolList* ptr = $6;
                         while (ptr != nullptr) {
                              symbolTable.remove(*ptr->symbol);
                              SymbolList* next = ptr->next;
                              delete ptr;
                              ptr = next;
                         }
                         symbolTable.exitScope(); 
                         if(symbolTable.contains($3)) {
                              yyerror("Function declaration invalid because symbol already exists!");
                         }
                         symbolTable.add($3, TypeNms::strToType($2), SymbolData::Function);
                         $$ = symbolTable.find(symbolTable.currentScope() + $3);
                    }
                    | FN CLASS ID ID {symbolTable.enterScope($4);} '(' fn_param ')' {
                         SymbolList* ptr = $7;
                         while (ptr != nullptr) {
                              symbolTable.remove(*ptr->symbol);
                              SymbolList* next = ptr->next;
                              delete ptr;
                              ptr = next;
                         }
                         // todo: check if class id is even declared
                         symbolTable.exitScope(); 
                         if(symbolTable.contains($4)) {
                              yyerror("Function declaration invalid because symbol already exists!");
                         }
                         symbolTable.add($4, TypeNms::Type::CUSTOM, SymbolData::Function);
                         $$ = symbolTable.find(symbolTable.currentScope() + $4);
                    }
                    ;

// verify if this function definition exists and looks the same
function_definition : FN TYPE ID {symbolTable.enterScope($3);} '(' fn_param ')' '{' statement_list '}' {
                         // TODO : IMPLEMENT HERE
                         symbolTable.exitScope();
                         SymbolData* fnData = symbolTable.find(symbolTable.currentScope() + $3);
                         if (fnData == nullptr) {
                              yyerror("No such function exists");
                         }

                         // check if signature of fn is same here as in decl
                    }
                    | FN CLASS ID ID {symbolTable.enterScope($4);} '(' fn_param ')' '{' statement_list '}' {
                         // TODO : IMPLEMENT HERE
                         symbolTable.exitScope();
                         SymbolData* fnData = symbolTable.find(symbolTable.currentScope() + $4);
                         if (fnData == nullptr) {
                              yyerror("No such function exists");
                         }
                         
                         // check if signature of fn is same here as in decl
                    }
                    ;

globals_block: decl SEP globals_block
             | /* epsilon */
	        ;
      

block : '{' {symbolTable.enterAnonymousScope(); } statement_list '}' { symbolTable.exitScope();} 
     ;

//what can be assigned to?
identifier: ID
          | ID '.' ID {/*class member*/}
          | ID '[' INTVAL ']' {/* array member*/}
          | SELF '.' ID {/*class member*/}
          ;

decl : decl_only
     | decl_assign                                             
     | function_declaration {}
     ;

decl_only: TYPE ID { 
               if(symbolTable.contains($2)) {
                    yyerror("Base type symbol could not be created because symbol name already exists!");
               }
               symbolTable.add($2, TypeNms::strToType($1), SymbolData::Variable);
               $$ = symbolTable.find(symbolTable.currentScope() + $2);
          }
          | ARRAY TYPE ID '[' INTVAL ']' { /* array decl */
               if(symbolTable.contains($3)) {
                    yyerror("Array type symbol could not be created because symbol name already exists!");
               }
               symbolTable.add($3, TypeNms::strToType($2), SymbolData::Variable, $5);
               $$ = symbolTable.find(symbolTable.currentScope() + $3);
          }
          | CLASS ID ID  { /*instante de clase. class MyClass x*/
               if(symbolTable.contains($3)) {
                    yyerror("Class type symbol could not be created because symbol name already exists!");
               }
               symbolTable.add($3, TypeNms::Type::CUSTOM, SymbolData::Variable);
               $$ = symbolTable.find(symbolTable.currentScope() + $3);
          }


// todo: DON'T FORGET TO MAKE EXPR WORK
decl_assign    : TYPE ID ASSIGN expr {
                    if(symbolTable.contains($2)) {
                         yyerror("Symbol already exists!");
                    }
                    symbolTable.add($2, TypeNms::strToType($1), SymbolData::Variable);
                    $$ = symbolTable.find(symbolTable.currentScope() + $2);
               }
               | CONST TYPE ID ASSIGN expr {
                    if(symbolTable.contains($2)) {
                         yyerror("Symbol already exists!");
                    }
                    symbolTable.add($3, TypeNms::strToType($2), SymbolData::Variable);
                    $$ = symbolTable.find(symbolTable.currentScope() + $3);
               } 
               | CLASS ID ID ASSIGN initializer_list {
                    /**/
               }
               | CONST CLASS ID ID ASSIGN initializer_list {}
               | ARRAY ID ID ASSIGN initializer_list {}
               | CONST ARRAY ID ID ASSIGN initializer_list {}



fn_param  : list_param { $$ = $1; }
          | /* epsilon */ { $$ = nullptr; }
          ;

list_param     : decl_only {
                    $$ = new SymbolList;
                    $$->next = nullptr;
                    $$->symbol = $1;
               }
               | decl_only ',' list_param {
                    $$ = new SymbolList;
                    $$->next = $3;
                    $$->symbol = $1;
               }
               ;

statement_list : stmt statement_list
               | /* epsilon */
               ;

stmt : sep_stmt SEP
     | non_sep_stmt
     ;

sep_stmt  : assignment {}
          | RETURN expr {/*expr is of func type?*/}
          | decl_assign {}
          | decl_only {}
          | EVAL '(' expr ')' {/* get value of expr as string, cout*/}
          | TYPEOF '(' expr ')' {/*get typeToStr*/}
          | DO ':' block WHILE expr {/*check if expr bool*/} 
          | expr { /*kinda just ignore*/}

non_sep_stmt   : if_expr ELSE ':' block {} 
               | if_expr {}
               | WHILE expr { /* check if expr bool */ }':' block
               | FOR assignment {} SEP expr {/*check if expr bool */ } SEP assignment ':'  {} block
               | block {}

if_expr   : IF expr { /* verify expr */ } ':' block
          ;

assignment: identifier ASSIGN expr {/*check if type of AST is type of identifier*/}
          ;
        
initializer_list : '{' expr_list '}'

expr_list : expr_list_ext
          | /* epsilon */
          ;

expr_list_ext  : expr ',' expr_list_ext { }
               | expr {/**/}

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
     |      MINUS expr {}
     | INCREMENT identifier {}
     | DECREMENT identifier {}
     | identifier INCREMENT {}
     | identifier DECREMENT {}
     | INTVAL {  std::cout << "Int Literal: " << $1 << std::endl;}
     | FLOATVAL { std::cout << "Float Literal: " << $1 << std::endl;} 
     | BOOLVAL { std::cout << "Bool Literal: " << $1 << std::endl; }
     | STRINGVAL { std::cout << "String Literal: " << $1 << std::endl; }
     | CHARVAL { std::cout << "Char Literal: " << $1 << std::endl;}
     | identifier { /* I say we allow everything, it will complain when comparing types -- asta am gandit si scrisesem altceva; da*/}
     | ID '(' expr_list ')' {/* aici nu trebuie sa verificam valoarea functiei, ci doar sa punem Default AST Node with the same type as function*/}  /* function calls */
     ;

%%
void yyerror(const char * s) {
     printf("Error: %s at line: %d\n",s,yylineno);
     exit(1);
}

int main(int argc, char** argv) {
     yyin=fopen(argv[1],"r");
     yyparse();
     std::cout << "\n\n--- SYMBOL TABLE ---\n\n" << std::endl;
     symbolTable.print(std::cout);
} 