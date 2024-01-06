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

    AST(const AST* other);
    AST(int literal);
    AST(float literal);
    AST(const char* literal);
    AST(char literal);
    AST(bool literal);
    AST(const SymbolData& symbol);

    // assignment as an operator.. ar trebui doar sa setam gen value ul symbol data ului la value ul din expr. ...nu?
    AST(SymbolData& symbol, const AST& left);

    std::string typeStr() const; // pt typeof 
    std::string valueStr() const; // pt eval

    TypeNms::Type type() const {
        return _type; //todo: move to cpp
    }
private:
    TypeNms::Type _type;
    SymbolData::Value _value;
    // nu stiu cum face yacc scope-urile
    // nu cred ca pot pune referinta aici
    const AST *_left = nullptr, *_right = nullptr;
};

#endif