#include <iostream>
#include "types.h"
#include "SymbolTable.h"

class AST {
public:
    // math expr
    AST(AST& left, OP op, AST& right);

    // cmp expr
    AST(AST& left, Comparison cmp, AST& right);

    // integer
    AST(int num);

    // float
    AST(float num);

    // assignment
    AST(SymbolData* symbol, )




private:

}