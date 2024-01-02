#ifndef _AST__
#define _AST__

#include <iostream>
#include <cmath>
#include <exception>
#include "SymbolTable.h"

class AST {
public:
    AST() = default;
    ~AST();
    AST(const AST& other) = default;
    AST& operator = (const AST& rhs);
    // expr
    // aparent asta obtii de la yacc, AST*&
    AST(Operation::BinaryOp op, const AST*& left, const AST*& right);
    AST(Operation::UnaryOp op, const AST*& left);

    AST(AST* other);
    AST(int literal);
    AST(float literal);
    AST(std::string literal);
    AST(char literal);
    AST(bool literal);
    AST(const SymbolData& symbol);

    // assignment as an operator?
    AST(SymbolData& symbol, const AST& left);

    std::string typeStr() const; // pt typeof 
    std::string valueStr() const; // pt eval
private:
    TypeNms::Type type;
    SymbolData::Value value;
    // nu stiu cum face yacc scope-urile
    // nu cred ca pot pune referinta aici
    const AST* left, *right;
};

#endif