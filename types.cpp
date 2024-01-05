#include "types.h"

TypeNms::Type TypeNms::strToType(const std::string& str) {
    // :)
    return std::unordered_map<std::string, Type> {
        {"int", INT},
        {"float", FLOAT},
        {"char", CHAR},
        {"string", STRING},
        {"bool", BOOL},
    }.at(str);
}

std::string TypeNms::typeToStr(const Type type) {
    switch (type) {
        case INT:
            return "Int";
            break;
        case FLOAT:
            return "Float";
            break;
        case CHAR:
            return "Char";
            break;
        case STRING:
            return "String";
            break;
        case BOOL:
            return "Bool";
            break;
        case CUSTOM:
            return "Class type";
            break;
    }
    // unreachable
    throw "Invalid argument";
}

// manipulate booleans
bool Operation::booleanOperator(const BinaryOp op) {
    // I love using c++20 features xD
    using enum BinaryOp;
    return std::unordered_set<BinaryOp>{
        ORB, ANDB, EQ, NEQ
    }.contains(op);
}

bool Operation::booleanOperator(const UnaryOp op) {
    using enum UnaryOp;
    return std::unordered_set<UnaryOp>{
        NEGB
    }.contains(op);
}

// cast to boolean
bool Operation::conversionOperator(const BinaryOp op) {
    using enum BinaryOp;
    return std::unordered_set<BinaryOp>{
        LT, LEQ, GT, GEQ, EQ, NEQ
    }.contains(op);
}

// same type result
bool Operation::expressionOperator(const BinaryOp op) {
    return !booleanOperator(op) && !conversionOperator(op);
}

bool Operation::expressionOperator(const UnaryOp op) {
    return !booleanOperator(op);
}


