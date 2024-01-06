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
void yyerror(std::string s);

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
%type<symbolValue> function_declaration decl decl_only decl_assign identifier

// am gasit asta pe net. sper sa nu ne strice mai tare
// daca te enerveaza warningurile legate de unused value da le comment
%destructor { free($$); } <stringValue>
%destructor { free($$); } <idValue>

%left LEQ GEQ LT GT EQ NEQ
%right NEGB
%left ANDB ORB
%left PLUS MINUS
%left DIV MUL
%left POW

%start start_program
%%

start_program : progr { std::cout << "The program is syntactically and semantically correct.\n";}

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

class_definition: CLASS ID '{' {
                         if (symbolTable.findClass($2) != nullptr) {
                              yyerror(std::string("Class ") + $2 + " already defined");
                         }
                         symbolTable.addClass($2);
                         symbolTable.enterScope($2);
                    } DECL '{' class_members '}' DEF '{' class_methods_def '}' '}' {symbolTable.exitScope();};

class_members: decl SEP class_members
             | /* epsilon */
             ;

// no sep needed when blocks are used
class_methods_def   : function_definition class_methods_def {}
                    | /* epsilon */

function_declaration: FN TYPE ID {symbolTable.enterScope($3); } '(' fn_param ')' {
                         // all fn params were added, succesfully, now add them to the functions data and remove them
                         SymbolList* ptr = $6;
                         std::vector<SymbolData> symbols;
                         while (ptr != nullptr) {
                              symbols.push_back(*ptr->symbol);
                              // check if maybe symbol with same name isn't already in symbols..?
                              symbolTable.remove(*ptr->symbol);
                              SymbolList* next = ptr->next;
                              delete ptr;
                              ptr = next;
                         }
                         symbolTable.exitScope(); 
                         if(symbolTable.contains($3)) {
                              yyerror(std::string("Function declaration invalid because symbol ") + $3 + " already exists");
                         }
                         symbolTable.add($3, TypeNms::strToType($2), SymbolData::Function);
                         $$ = symbolTable.find(symbolTable.currentScope() + $3);
                         for (const auto& sym : symbols) {
                              $$->addSymbol(sym);
                         }
                    }
                    | FN CLASS ID ID {symbolTable.enterScope($4);} '(' fn_param ')' {
                         SymbolList* ptr = $7;
                         std::vector<SymbolData> symbols;
                         while (ptr != nullptr) {
                              symbols.push_back(*ptr->symbol);
                              symbolTable.remove(*ptr->symbol);
                              SymbolList* next = ptr->next;
                              delete ptr;
                              ptr = next;
                         }
                         if (symbolTable.findClass($3) == nullptr) {
                              yyerror(std::string("Cannot declare function ") + $4 + " with undeclared class return-type " + $3);
                         }
                         symbolTable.exitScope(); 
                         if(symbolTable.contains($4)) {
                              yyerror(std::string("Function declaration invalid because symbol ") + $4 + " already exists");
                         }
                         symbolTable.add($4, TypeNms::Type::CUSTOM, SymbolData::Function, 0, $3);
                         $$ = symbolTable.find(symbolTable.currentScope() + $4);
                         for (const auto& sym : symbols) {
                              $$->addSymbol(sym);
                         }
                    }
                    ;

// verify if this function definition exists and looks the same
function_definition : FN TYPE ID {symbolTable.enterScope($3);} '(' fn_param ')' '{' statement_list '}' {
                         //TODO : IMPLEMENT HERE
                         SymbolList* ptr = $6;
                         std::vector<SymbolData> symbols;
                         while (ptr != nullptr) {
                              symbols.push_back(*ptr->symbol);
                              symbolTable.remove(*ptr->symbol);
                              SymbolList* next = ptr->next;
                              delete ptr;
                              ptr = next;
                         }
                         symbolTable.exitScope();
                         SymbolData* fnData = symbolTable.find(symbolTable.currentScope() + $3);
                         if (fnData == nullptr) {
                              yyerror(std::string("No function ") + $3 + " exists in current scope");
                         }
                         SymbolData tempFnSymbol(symbolTable.currentScope(), $3, TypeNms::strToType($2), SymbolData::Flag::Function);
                         for (const auto& sym : symbols) {
                              tempFnSymbol.addSymbol(sym);
                         }
                         if (!(fnData->hasSameTypeAs(tempFnSymbol))) {
                              yyerror(std::string("Definition of function ") + $3 + " has different signature than that of its declaration");
                         }
                    }
                    | FN CLASS ID ID {symbolTable.enterScope($4);} '(' fn_param ')' '{' statement_list '}' {
                         // TODO : IMPLEMENT HERE
                         symbolTable.exitScope();
                         SymbolData* fnData = symbolTable.find(symbolTable.currentScope() + $4);
                         if (fnData == nullptr) {
                              yyerror(std::string("No function ") + $3 + " exists in current scope");
                         }
                         // etc
                    }
                    ;

globals_block: decl SEP globals_block
             | /* epsilon */
	        ;
      

block : '{' {symbolTable.enterAnonymousScope(); } statement_list '}' { symbolTable.exitScope();} 
     ;

identifier: ID { $$ = symbolTable.findId($1); if (!$$) yyerror(std::string("Undefined identifier ") + $1); }
          | ID '.' ID {
               // find class variable
               SymbolData* var = symbolTable.findId($1);
               if (var == nullptr) yyerror(std::string("Cannot access undefined symbol ") + $1);
               $$ = var->member($3);
               if ($$ == nullptr) yyerror(std::string("Cannot access undefined member ") + $3 + " of symbol " + $1);
          }
          | ID '[' INTVAL ']' {
               SymbolData* var = symbolTable.findId($1);
               if (var == nullptr) yyerror(std::string("Undefined symbol ") + $1);
               $$ = var->member($3);
               if ($$ == nullptr) yyerror(std::string("Array index out of bounds for symbol ") + $1);
          }
          | SELF '.' ID {/* il lasam asa. methods are only syntactic anyway si n avem self decat in methods */}
          ;

decl : decl_only
     | decl_assign                                             
     | function_declaration {}
     ;

decl_only: TYPE ID { 
               if(symbolTable.contains($2)) {
                    yyerror(std::string("Base type symbol could not be created because symbol ") + $2 + " already exists");
               }
               symbolTable.add($2, TypeNms::strToType($1), SymbolData::Variable);
               $$ = symbolTable.find(symbolTable.currentScope() + $2);
          }
          | ARRAY TYPE ID '[' INTVAL ']' { /* array decl */
               if(symbolTable.contains($3)) {
                    yyerror(std::string("Class type symbol could not be created because symbol ") + $3 + " already exists");
               }
               symbolTable.add($3, TypeNms::strToType($2), SymbolData::Variable, $5);
               $$ = symbolTable.find(symbolTable.currentScope() + $3);
          }
          | CLASS ID ID  { /*instante de clase. class MyClass x*/
               if(symbolTable.contains($3)) {
                    yyerror(std::string("Class type symbol could not be created because symbol ") + $3 + " already exists");
               }
               // check if class is defined
               if (symbolTable.findClass($2) == nullptr) {
                    yyerror(std::string("Cannot instantiate symbol ") + $3 + " of non-defined class type " + $2);
               }
               symbolTable.add($3, TypeNms::Type::CUSTOM, SymbolData::Variable, 0, $2); // added class name
               $$ = symbolTable.find(symbolTable.currentScope() + $3);
          }


// todo: DON'T FORGET TO MAKE EXPR WORK
decl_assign    : TYPE ID ASSIGN expr {
                    if(symbolTable.contains($2)) {
                         yyerror(std::string("Symbol ") + $2 + " already exists");
                    }
                    symbolTable.add($2, TypeNms::strToType($1), SymbolData::Variable);
                    $$ = symbolTable.find(symbolTable.currentScope() + $2);
               }
               | CONST TYPE ID ASSIGN expr {
                    if(symbolTable.contains($3)) {
                         yyerror(std::string("Symbol ") + $3 + " already exists");
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
          | EVAL '(' expr ')' {std::cout << "\033[1;32mEVAL: " << $3->valueStr() << "\033[0m\n";}
          | TYPEOF '(' expr ')' {std::cout << "\033[1;36mTYPEOF: " << $3->typeStr() << "\033[0m\n";}
          | DO ':' block WHILE expr {std::cout << "here\n";} 
          | expr { /*kinda just ignore*/}

non_sep_stmt   : if_expr ELSE ':' block {} 
               | if_expr {}
               | WHILE expr { /* check if expr bool */ }':' block
               | FOR assignment {} SEP expr {/*check if expr bool */ } SEP assignment ':'  {} block
               | block {}

if_expr   : IF expr { /* verify expr */ } ':' block
          ;

assignment: identifier ASSIGN expr {delete $3;} // rip
          ;
        
initializer_list : '{' expr_list '}'

expr_list : expr_list_ext
          | /* epsilon */
          ;

expr_list_ext  : expr ',' expr_list_ext { }
               | expr {/**/}

expr : '(' expr ')' {$$ = new AST($2);}
     | expr PLUS  expr { $$ = new AST(Operation::BinaryOp::PLUS, $1, $3);}
     | expr MINUS expr { $$ = new AST(Operation::BinaryOp::MINUS, $1, $3);}
     | expr  MUL  expr { $$ = new AST(Operation::BinaryOp::MULT, $1, $3);}
     | expr  DIV  expr { $$ = new AST(Operation::BinaryOp::DIV, $1, $3);}
     | expr  POW  expr { $$ = new AST(Operation::BinaryOp::POW, $1, $3);}
     | expr ANDB  expr { $$ = new AST(Operation::BinaryOp::ANDB, $1, $3);}
     | expr  ORB  expr { $$ = new AST(Operation::BinaryOp::ORB, $1, $3);}
     | expr  EQ   expr { $$ = new AST(Operation::BinaryOp::EQ, $1, $3);}
     | expr  NEQ  expr { $$ = new AST(Operation::BinaryOp::NEQ, $1, $3);}
     |      NEGB  expr { $$ = new AST(Operation::UnaryOp::NEGB, $2);}
     | expr  LT   expr { $$ = new AST(Operation::BinaryOp::LT, $1, $3);}
     | expr  LEQ  expr { $$ = new AST(Operation::BinaryOp::LEQ, $1, $3);}
     | expr  GT   expr { $$ = new AST(Operation::BinaryOp::GT, $1, $3);}
     | expr  GEQ  expr { $$ = new AST(Operation::BinaryOp::GEQ, $1, $3);}
     |      MINUS expr { $$ = new AST(Operation::BinaryOp::MINUS, $2, $2);}
     | INCREMENT identifier {/*tu le ai fct tu te ocupi*/}
     | DECREMENT identifier {}
     | identifier INCREMENT {}
     | identifier DECREMENT {}
     | INTVAL {  std::cout << "Int Literal: " << $1 << std::endl;  $$ = new AST((int)$1); } 
     | FLOATVAL { std::cout << "Float Literal: " << $1 << std::endl; $$ = new AST((float)$1); } 
     | BOOLVAL { std::cout << "Bool Literal: " << $1 << std::endl; $$ = new AST((bool)$1); }
     | STRINGVAL { std::cout << "String Literal: " << $1 << std::endl; $$ = new AST($1); }
     | CHARVAL { std::cout << "Char Literal: " << $1 << std::endl; $$ = new AST($1); }
     | identifier { /* TEMPORAR!!!!!! todo: change */ $$ = new AST((int)0);}
     | ID '(' expr_list ')' {/* aici nu trebuie sa verificam valoarea functiei, ci doar sa punem Default AST Node with the same type as function*/}  /* function calls */
     ;

%%
// s-ar putea sa vrem si din asta
void yywarning(const char* s) {

}
void yyerror(std::string s) {
     printf("\033[1;31mError:\033[0m %s \033[1;36m(at line: %d)\033[0m\n", s.c_str(), yylineno);
     exit(1);
}

int main(int argc, char** argv) {
     yyin=fopen(argv[1],"r");
     try {
          yyparse();
     }
     catch(std::exception& e) {
          yyerror(e.what());
     }
     std::cout << "\n\n--- SYMBOL TABLE ---\n\n" << std::endl;
     symbolTable.print(std::cout);
} 