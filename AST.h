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

    AST(const AST* other);
    AST(int literal);
    AST(float literal);
    AST(const char* literal);
    AST(char literal);
    AST(bool literal);
    AST(const SymbolData& symbol);


    std::string typeStr() const; // pt typeof 
    std::string valueStr() const; // pt eval

    const SymbolData& symbol() const;

private:
    SymbolData _symbol;
    const AST *_left = nullptr, *_right = nullptr;
};
