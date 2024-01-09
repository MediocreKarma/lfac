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
void yyerror(const std::string& s);

struct SymbolList {
     SymbolList* next;
     SymbolData* symbol;
};

struct SizeList {
     SizeList* next;
     size_t size;
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
     struct SizeList* sizeList;
}
%token CLASSES ENDCLASSES FUNCTIONS ENDFUNCTIONS GLOBALS ENDGLOBALS MAIN
%token SEP ASSIGN DEF DECL MEMBERS MEMBER
%token WHILE FOR IF ELSE DO RETURN EVAL TYPEOF
%token CLASS CONST FN SELF
%token<idValue> TYPE ID
%token<stringValue> STRINGVAL
%token<intValue> INTVAL
%token<floatValue> FLOATVAL
%token<boolValue> BOOLVAL
%token<charValue> CHARVAL
%type<astNode> expr
%type<list> list_param fn_param
%type<symbolValue> function_declaration decl decl_only decl_assign identifier assignment initializer_list initializer_list_inner initializer_list_elem expr_list expr_list_ext expr_list_elem
%type<list> class_members class_members1
%type<sizeList> index_list

%right ASSIGN
%left ANDB ORB
%left LEQ GEQ LT GT EQ NEQ
%left PLUS MINUS
%left DIV MUL
%left POW
%left MEMBER
%left '[' ']'
%right NEGB

%start start_program
%%

start_program : progr { std::cout << "The program is syntactically and semantically correct.\n";}

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

class_definition:   CLASS ID '{' {
                         if (symbolTable.findClass($2) != nullptr) {
                              yyerror(std::string("Class ") + $2 + " already defined");
                         }
                         symbolTable.addClass($2);
                         symbolTable.enterScope($2);
                    } MEMBERS '{' class_members '}' DECL '{' functions_decl '}' DEF '{' class_methods_def '}' '}' {
                         SymbolData* classDef = symbolTable.findClass($2);
                         SymbolList* ptr = $7;
                         while (ptr != nullptr) {
                              classDef->addSymbol(*ptr->symbol);
                              symbolTable.remove(*ptr->symbol);
                              SymbolList* next = ptr->next;
                              delete ptr;
                              ptr = next;
                         }
                         symbolTable.exitScope();
                    };

class_members  : class_members1 { $$ = $1; }
               | /* epsilon */ { $$ = nullptr; }
               ;

class_members1 : decl_assign SEP {
                    $$ = new SymbolList;
                    $$->next = nullptr;
                    $$->symbol = $1;
               }
               | decl_assign SEP class_members1 {
                    $$ = new SymbolList;
                    $$->next = $3;
                    $$->symbol = $1;
               }
               | decl_only SEP {
                    $$ = new SymbolList;
                    $$->next = nullptr;
                    $$->symbol = $1;
               }
               | decl_only SEP class_members1 {
                    $$ = new SymbolList;
                    $$->next = $3;
                    $$->symbol = $1;
               }
               ;

class_methods_def   : function_definition class_methods_def {}
                    | /* epsilon */

function_declaration: FN TYPE ID {symbolTable.enterScope($3); } '(' fn_param ')' {
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
                         if(symbolTable.contains($3)) {
                              yyerror(std::string("Function declaration invalid because symbol ") + $3 + " already exists in same scope");
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
                         SymbolData* classDef = symbolTable.findClass($3);
                         if (classDef == nullptr) {
                              yyerror(std::string("Cannot declare function ") + $4 + " with undeclared class return-type " + $3);
                         }
                         symbolTable.exitScope(); 
                         if(symbolTable.contains($4)) {
                              yyerror(std::string("Function declaration invalid because symbol ") + $4 + " already exists in same scope");
                         }
                         symbolTable.add($4, TypeNms::Type::CUSTOM, SymbolData::Function, {}, classDef);
                         $$ = symbolTable.find(symbolTable.currentScope() + $4);
                         for (const auto& sym : symbols) {
                              $$->addSymbol(sym);
                         }
                    }
                    ;

function_definition : FN TYPE ID {symbolTable.enterScope($3);} '(' fn_param ')' '{' statement_list '}' {
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
                         SymbolData* classDef = symbolTable.findClass($3);
                         SymbolList* ptr = $7;
                         std::vector<SymbolData> symbols;
                         while (ptr != nullptr) {
                              symbols.push_back(*ptr->symbol);
                              symbolTable.remove(*ptr->symbol);
                              SymbolList* next = ptr->next;
                              delete ptr;
                              ptr = next;
                         }
                         symbolTable.exitScope();
                         SymbolData* fnData = symbolTable.find(symbolTable.currentScope() + $4);
                         if (fnData == nullptr) {
                              yyerror(std::string("No function ") + $4 + " exists in current scope");
                         }
                         SymbolData tempFnSymbol(symbolTable.currentScope(), $4, TypeNms::Type::CUSTOM, SymbolData::Flag::Function, {}, classDef);
                         for (const auto& sym : symbols) {
                              tempFnSymbol.addSymbol(sym);
                         }
                         if (!(fnData->hasSameTypeAs(tempFnSymbol))) {
                              yyerror(std::string("Definition of function ") + $4 + " has different signature than that of its declaration");
                         }
                    }
                    ;

globals_block: decl SEP globals_block
             | /* epsilon */
	        ;
      

block : '{' {symbolTable.enterAnonymousScope(); } statement_list '}' { symbolTable.exitScope();} 
     ;

identifier: ID { 
               $$ = symbolTable.findId($1); if (!$$) yyerror(std::string("Undefined identifier ") + $1); 
          }
          | identifier MEMBER ID {
               $$ = $1->member($3);
               if ($$ == nullptr) yyerror(std::string("Cannot access undefined member ") + $3 + " of symbol " + $1->name());
          }
          | identifier '[' INTVAL ']' {
               $$ = $1->member($3);
               if ($$ == nullptr) yyerror(std::string("Array index out of bounds for symbol ") + $1->name());
          }
          | SELF MEMBER identifier {
               std::string scope = symbolTable.currentScope();
               std::string base = scope.substr(1, scope.find_first_of('/', 1) - 1);
               SymbolData* cl = symbolTable.findClass(base);
               if (cl->member($3->name())) {
                    yyerror("Cannot access undefined member of current class");
               }
               $$ = $3;
          }
          ;

decl : decl_only
     | decl_assign {}                                         
     | function_declaration {}
     ;

decl_only: TYPE ID { 
               if(symbolTable.contains($2)) {
                    yyerror(std::string("Base type symbol could not be created because symbol ") + $2 + " already exists in same scope");
               }
               symbolTable.add($2, TypeNms::strToType($1), SymbolData::Variable);
               $$ = symbolTable.find(symbolTable.currentScope() + $2);
          }
          | TYPE ID index_list {
               if(symbolTable.contains($2)) {
                    yyerror(std::string("Array type symbol could not be created because symbol ") + $2 + " already exists in same scope");
               }
               std::vector<size_t> indexes;
               SizeList* ptr = $3;
               while (ptr != nullptr) {
                    indexes.push_back(ptr->size);
                    SizeList* next = ptr->next;
                    delete ptr;
                    ptr = next;
               }
               symbolTable.add($2, TypeNms::strToType($1), SymbolData::Variable, indexes);
               $$ = symbolTable.find(symbolTable.currentScope() + $2);
          }
          | CLASS ID ID {
               std::cout << "here\n";
               if(symbolTable.contains($3)) {
                    yyerror(std::string("Class type symbol could not be created because symbol ") + $3 + " already exists in same scope");
               }
               SymbolData* classSymbol = symbolTable.findClass($2);
               if (classSymbol == nullptr) {
                    yyerror(std::string("Cannot instantiate symbol ") + $3 + " of non-defined class type " + $2);
               }
               SymbolData symbol = classSymbol->instantiateClass(symbolTable.currentScope(), $3);
               symbolTable.add(symbol);
               $$ = symbolTable.find(symbolTable.currentScope() + $3);
               std::cout << "here\n";
          }
          | CLASS ID ID index_list {
               SymbolData* classDef = symbolTable.findClass($2);
               if (classDef == nullptr) {
                    yyerror(std::string("Cannot initialize class of undefined class ") + $2);
               } 
               if(symbolTable.contains($3)) {
                    yyerror(std::string("Array type symbol could not be created because symbol ") + $3 + " already exists in same scope");
               }
               std::vector<size_t> indexes;
               SizeList* ptr = $4;
               while (ptr != nullptr) {
                    indexes.push_back(ptr->size);
                    SizeList* next = ptr->next;
                    delete ptr;
                    ptr = next;
               }
               symbolTable.add($3, TypeNms::Type::CUSTOM, SymbolData::Variable, indexes, classDef);
               $$ = symbolTable.find(symbolTable.currentScope() + $3);
          }

decl_assign    : TYPE ID ASSIGN expr {
                    if(symbolTable.contains($2)) {
                         yyerror(std::string("Symbol ") + $2 + " already exists in same scope");
                    }
                    symbolTable.add($2, TypeNms::strToType($1), SymbolData::Variable);
                    $$ = symbolTable.find(symbolTable.currentScope() + $2);
                    $$->assign($4->symbol());
               }
               | CONST TYPE ID ASSIGN expr {
                    if(symbolTable.contains($3)) {
                         yyerror(std::string("Symbol ") + $3 + " already exists in same scope");
                    }
                    symbolTable.add($3, TypeNms::strToType($2), SymbolData::Variable);
                    $$ = symbolTable.find(symbolTable.currentScope() + $3);
                    $$->assign($5->symbol());
                    $$->setConst();

               } 
               | CLASS ID ID ASSIGN initializer_list {
                    if(symbolTable.contains($3)) {
                         yyerror(std::string("Class type symbol could not be created because symbol ") + $3 + " already exists in same scope");
                    }
                    SymbolData* classSymbol = symbolTable.findClass($2);
                    if (classSymbol == nullptr) {
                         yyerror(std::string("Cannot instantiate symbol ") + $3 + " of non-defined class type " + $2);
                    }
                    SymbolData symbol = classSymbol->instantiateClass(symbolTable.currentScope(), $3);
                    symbolTable.add(symbol);
                    $$ = symbolTable.find(symbolTable.currentScope() + $3);
                    $$->assign($5->value());

                    delete $5;
               }
               | CONST CLASS ID ID ASSIGN initializer_list {
                    if(symbolTable.contains($4)) {
                         yyerror(std::string("Class type symbol could not be created because symbol ") + $4 + " already exists in same scope");
                    }
                    SymbolData* classSymbol = symbolTable.findClass($3);
                    if (classSymbol == nullptr) {
                         yyerror(std::string("Cannot instantiate symbol ") + $4 + " of non-defined class type " + $3);
                    }
                    SymbolData symbol = classSymbol->instantiateClass(symbolTable.currentScope(), $4);
                    symbolTable.add(symbol);
                    $$ = symbolTable.find(symbolTable.currentScope() + $4);
                    $$->assign($6->value());
                    $$->setConst();
                    delete $6;
               }
               // todo: do these
               //| ID ID ASSIGN initializer_list {}
               //| CONST ID ID ASSIGN initializer_list {}

index_list: '[' INTVAL ']' {
               $$ = new SizeList;
               $$->next = nullptr;
               $$->size = $2;
          }
          | '[' INTVAL ']' index_list {
               $$ = new SizeList;
               $$->next = $4;
               $$->size = $2;
          }
          ;


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
          | RETURN expr {/*
               todo: check if expr is of func type
          */}
          | decl_assign {}
          | decl_only {}
          | function_call {}
          | EVAL '(' expr ')' {std::cout << "\033[1;32mEVAL: " << $3->valueStr() << "\033[0m\n";}
          | TYPEOF '(' expr ')' {std::cout << "\033[1;36mTYPEOF: " << $3->typeStr() << "\033[0m\n";}
          | DO ':' block WHILE expr {if ($5->symbol().type() != TypeNms::BOOL) {yyerror("Condition of do-while loop has to be of boolean type");}} 

non_sep_stmt   : if_expr ELSE ':' block {} 
               | if_expr {}
               | WHILE expr { if ($2->symbol().type() != TypeNms::BOOL) {yyerror("Condition of while loop has to be of boolean type");}}':' block
               | FOR assignment {} SEP expr { if ($5->symbol().type() != TypeNms::BOOL) {yyerror("Condition of for loop has to be of boolean type");}} SEP assignment ':'  {} block
               | block {}

if_expr   : IF expr {  if ($2->symbol().type() != TypeNms::BOOL) {yyerror("Condition of if expression has to be of boolean type");}} ':' block
          ;

assignment: identifier ASSIGN expr {
                    $$->assign($3->symbol());
                    delete $3;
               }
          ;

function_call  : ID '(' expr_list ')' {
                    auto fxnSymbol = symbolTable.findId($1);
                    if (!fxnSymbol) {
                         yyerror("Cannot call non-existent function symbol");
                    }
                    if (!fxnSymbol->isFunc()) {
                         yyerror("Cannot call non-function symbol");
                    }
                    auto fxnVal = std::get<std::vector<SymbolData>>(fxnSymbol->value());
                    auto exprVal = std::get<std::vector<SymbolData>>($3->value());

                    if (fxnVal.size() != exprVal.size()) {
                         yyerror("Wrong number of parameters");
                    }

                    for (size_t i = 0; i < fxnVal.size(); ++i) {
                         if (!sameType(fxnVal[i], exprVal[i])) {
                              yyerror(std::string("Cannot call function ") + fxnSymbol->name() + " with parameter at position " + std::to_string(i) + " of type " + TypeNms::typeToStr( exprVal[i].type()));
                         }
                    }
               }
               | identifier MEMBER ID '(' expr_list ')' {
                    if ($1->type() != TypeNms::CUSTOM) {
                         yyerror("Cannot call method of non-class symbol");
                    }
                    auto className = $1->className();
                    if (className.empty()) { 
                         yyerror("Cannot determine class name of symbol");
                    }
                    auto functionName = std::string($3);
                    auto methodScope = Scope::scopeToString({"", className});
                    auto scopedName = Scope::scopeWithNameToString(methodScope, functionName);
                    auto methodSymbol = symbolTable.find(scopedName);
                    if (!methodSymbol) {
                         yyerror("Cannot call non-existent class method " + scopedName);
                    }
                    auto fxnVal = std::get<std::vector<SymbolData>>(methodSymbol->value());
                    auto exprVal = std::get<std::vector<SymbolData>>($5->value());
                    
                    if (fxnVal.size() != exprVal.size()) {
                         yyerror("Wrong number of parameters");
                    }
                    for (size_t i = 0; i < fxnVal.size(); ++i) {
                         if (!sameType(fxnVal[i], exprVal[i])) {
                              yyerror(std::string("Cannot call function ") + methodSymbol->name() + " with parameter at position " + std::to_string(i) + " of type " + TypeNms::typeToStr( exprVal[i].type()));
                         }
                    }
               }
               ;
        
initializer_list : '{' initializer_list_inner  '}' {$$ = $2;}
                 ;

initializer_list_inner : initializer_list_elem {
                              $$ = new SymbolData("", "", TypeNms::CUSTOM, SymbolData::Flag::Variable);
                              $$->addSymbol(*$1);
                              delete $1;
                         }
                         | initializer_list_elem ',' initializer_list_inner {
                              $$ = $3;
                              $3->addSymbolToBeginning(*$1);
                              delete $1;
                         }
                         ;

initializer_list_elem : initializer_list {
                              $$ = $1;
                        }
                      | expr {
                              $$ = new SymbolData($1->symbol());
                              
                      }
                      ;


expr_list : expr_list_ext {$$ = $1;}
          | /* epsilon */ {$$ = new SymbolData("", "", TypeNms::CUSTOM, SymbolData::Flag::Variable);}
          ;

expr_list_ext  : expr_list_elem ',' expr_list_ext {
                    $$ = $3;
                    $3->addSymbolToBeginning(*$1);
                    delete $1;
               }
               | expr_list_elem {$$ = new SymbolData("", "", TypeNms::CUSTOM, SymbolData::Flag::Variable); $$->addSymbol(*$1);}

expr_list_elem : expr {$$ = new SymbolData($1->symbol());}

expr : '(' expr ')' {$$ = $2;}
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
     |      MINUS expr { $$ = new AST(Operation::UnaryOp::NEG, $2);}
     | INTVAL { $$ = new AST((int)$1); } 
     | FLOATVAL { $$ = new AST((float)$1); } 
     | BOOLVAL { $$ = new AST((bool)$1); }
     | STRINGVAL { $$ = new AST($1); }
     | CHARVAL { $$ = new AST($1); }
     | assignment { $$ = new AST(*$1);}
     | identifier { $$ = new AST(*$1);}
     | function_call {$$ = new AST((int)0);}
     ;

%%

void yyerror(const std::string& s) {
     Utils::printError(s);
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
     symbolTable.print(std::cout);
} 