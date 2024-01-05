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
    // nvm eu nu mai inteleg ce returneaza AST-urile astea...
    // cred ca defapt aveam vreo eroare tampita lol
    AST(Operation::BinaryOp op, AST*& left, AST*& right);
    AST(Operation::UnaryOp op, AST*& left);

    AST(AST* other);
    AST(int literal);
    AST(float literal);
    AST(const char* literal);
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
    AST* left, *right;
};

#endif