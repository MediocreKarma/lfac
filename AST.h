#ifndef _AST__
#define _AST__

#include <iostream>
#include "types.h"
#include "SymbolTable.h"

class AST {
public:
    // math expr
    AST(AST& left, BinaryOp op, AST& right);

    // cmp expr
    AST(UnaryOp op, AST& left);

    // integer
    AST(int num);

    // float
    AST(float num);

    // assignment
    AST(SymbolData& symbol, std::string data);




private:

};

#endif