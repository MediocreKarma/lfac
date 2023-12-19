#ifndef _AST__
#define _AST__

#include <iostream>
#include "types.h"
#include "SymbolTable.h"

class AST {
public:
    // expr 
    AST(const AST& left, BinaryOp op, const AST& right);
    AST(UnaryOp op, const AST& left);

    AST(int i);
    AST(float f);
    AST(std::string s);
    AST(char c);
    AST(bool b);

    // assignment as an operator?
    AST(SymbolData& symbol, const AST& left);
    
    




private:

};

#endif