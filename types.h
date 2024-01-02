#ifndef _TYPES__
#define _TYPES__

#include <string>
#include <vector>
#include <variant>
#include <unordered_map>
#include <unordered_set>

// find better naming? TypeNamespace xd
namespace TypeNms {
    enum Type {
        INT, FLOAT, CHAR, STRING, BOOL, CUSTOM
    };
    Type strToType(const std::string& str);
    std::string typeToStr(Type t);
}

namespace Operation {
    enum class BinaryOp {
        PLUS,
        MINUS,
        MULT,
        POW,
        DIV,
        ORB,
        ANDB,
        LT,
        LEQ,
        GT,
        GEQ,
        EQ,
        NEQ
    };

    enum class UnaryOp {
        NEGB,
        NEG
    };

    bool booleanOperator(BinaryOp op);
    bool booleanOperator(UnaryOp op);

    bool expressionOperator(BinaryOp op);
    bool expressionOperator(UnaryOp op);

    bool conversionOperator(BinaryOp op);
}

#endif 