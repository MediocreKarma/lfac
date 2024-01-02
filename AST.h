#ifndef _AST__
#define _AST__

#include <iostream>
#include <cmath>
#include "SymbolTable.h"

class AST {
public:
    constexpr AST() = default;
    AST(const AST& other) = default;
    AST& operator = (const AST& rhs);
    // expr
    AST(Operation::BinaryOp op, const AST& left, const AST& right);
    AST(Operation::UnaryOp op, const AST& left);

    AST(int i);
    AST(float f);
    AST(std::string s);
    AST(char c);
    AST(bool b);
    AST(const SymbolData& symbol);

    // assignment as an operator?
    AST(SymbolData& symbol, const AST& left);
    
    




private:
    TypeNms::Type type;
    SymbolData::Value value;
    // nu stiu cum face yacc scope-urile
    // nu cred ca pot pune referinta aici
    const AST* left, *right;
};

#endif