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
    AST(Operation::BinaryOp op, const AST* left, const AST* right);
    AST(Operation::UnaryOp op, const AST* left);

    AST(int literal);
    AST(float literal);
    AST(const char* literal);
    AST(char literal);
    AST(bool literal);
    AST(const SymbolData& symbol);

    TypeNms::Type type() const;

    std::string typeStr() const; // pt typeof 
    std::string valueStr() const; // pt eval

    SymbolData symbol() const;

    SymbolData evaluate() const;

private:

    SymbolData evaluateUnary() const;
    SymbolData evaluateBinary() const;

    SymbolData _symbol;
    
    TypeNms::Type _type;

    bool _isOperation = false;
    int _op;
    Operation::Type _operationType;

    const AST *_left = nullptr, *_right = nullptr;

};
