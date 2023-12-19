#ifndef _TYPES__
#define _TYPES__

#include <string>
#include <vector>
#include <variant>

class Type {
protected:
    std::string scope;
    bool is_init;
    bool is_const;
    // 0 if non array-type
    size_t array_size;

    Type(size_t array_size = 0);

    Type(const Type&);
    Type(Type&&);
    Type& operator = (const Type&);
    Type& operator = (Type&&);

};


class BuiltInType : Type {
    std::variant<int, float, char, bool, std::string> var;
    BuiltInType(const std::variant<int, float, char, bool, std::string>& var, size_t array_size = 0);
};

class CustomType : Type {
    std::string generatedScope;
    std::vector<Type*> variables;
};

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
    NEGB,

};

#endif 