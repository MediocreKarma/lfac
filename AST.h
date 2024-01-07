#pragma once

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
    AST(Operation::BinaryOp op, const AST* left, const AST* right);
    AST(Operation::UnaryOp op, const AST* left);

    // why is this needed? -- pt expresii cu paranteze
    AST(const AST* other);
    AST(int literal);
    AST(float literal);
    AST(const char* literal);
    AST(char literal);
    AST(bool literal);
    AST(const SymbolData& symbol);

    // assignment as an operator
    AST(SymbolData& symbol, const AST& left);

    std::string typeStr() const; // pt typeof 
    std::string valueStr() const; // pt eval

    const SymbolData::Value& value() const;

    const SymbolData& symbol() const;

private:
    SymbolData _symbol;
    // nu stiu cum face yacc scope-urile
    // nu cred ca pot pune referinta aici
    const AST *_left = nullptr, *_right = nullptr;
};
