#ifndef _TYPES__
#define _TYPES__

#include <string>
#include <vector>
#include <variant>
#include <unordered_map>

enum Type {
    INT, FLOAT, CHAR, STRING, BOOL, CUSTOM
};

Type typeFromString(const std::string& str);

enum class BinaryOp {
    PLUS,
    MINUS,
    MULT,
    POW,
    DIV,
    ORB,
    ANDB,
    LE,
    LEQ,
    GE,
    GEQ,
    EQ,
    NEQ
};

enum class UnaryOp {
    NEGB

};

#endif 