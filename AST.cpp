#include "AST.h"

AST::~AST() {
    delete left;
    delete right;
}

AST::AST(const AST* other) {
    using namespace TypeNms;
    type = other->type;
    value = other->value;
    left = other;
}

AST::AST(int literal) {
    using namespace TypeNms;
    type = INT;
    value = literal;
}

AST::AST(float literal) {
    using namespace TypeNms;
    type = FLOAT;
    value = (float)literal;
}

AST::AST(bool literal) {
    using namespace TypeNms;
    type = FLOAT;
    value = literal;
}

AST::AST(const char* literal) {
    using namespace TypeNms;
    type = STRING;
    value = std::string(literal);
}

AST::AST(char literal) {
    using namespace TypeNms;
    type = CHAR;
    value = literal;
}

AST::AST(const SymbolData& symbol) {
    type = symbol.type();
}

AST::AST(Operation::UnaryOp op, AST*& _left) :
    type(), value(), left(_left) {
    
    using namespace TypeNms;
    using enum Operation::UnaryOp;

    type = left->type;

    if (type != BOOL) {
        // yyerror
    }
    if (!Operation::booleanOperator(op)) {
        // yyerror
    }
    switch(op) {
        case NEGB:
            value = !std::get<bool>(left->value); 
            break;
    }
}

AST::AST(Operation::BinaryOp op, AST*& _left, AST*& _right) :
    type(), value(), left(_left), right(_right) {

    // todo: caz special pt minus ceva
    if (left == nullptr or right == nullptr) {
        throw std::runtime_error("Cannot construct binary-op AST with null values");
    }

    using namespace TypeNms;
    using enum Operation::BinaryOp;

    // no type casting
    if (left->type != right->type) {
        std::string error = 
            "Binary operation using " +
            typeToStr(left->type) + 
            " and " +
            typeToStr(right->type) +
            " is not allowed!"; 
        // yyerror is not yet here, kinda sucks?
        // yyerror(error.c_str());
    }
    if (left->value.index() != right->value.index()) {
        // disaster
        exit(1);
    }
    type = left->type;
    if (type == BOOL) {
        if (Operation::booleanOperator(op)) {
            switch (op) {
                case ORB:
                    value = std::get<bool>(left->value) || std::get<bool>(right->value);
                    break;
                case ANDB:
                    value = std::get<bool>(left->value) && std::get<bool>(right->value);
                    break;
                case EQ:
                    value = std::get<bool>(left->value) == std::get<bool>(right->value);
                    break;
                case NEQ:
                    value = std::get<bool>(left->value) != std::get<bool>(right->value);
                    break;
                default: /* unreachable */;
            }
            // good
            return;
        }
        std::string error = "Invalid operation for boolean type";
        // yyerror
    }
    else {
        if (Operation::conversionOperator(op)) {
            switch (op) {
                case LT:
                    switch (type) {
                        case INT: value = std::get<int>(left->value) < std::get<int>(right->value); break;
                        case FLOAT: value = std::get<float>(left->value) < std::get<float>(right->value); break;
                        case STRING: value = std::get<std::string>(left->value) < std::get<std::string>(right->value); break;
                        case CHAR: value = std::get<char>(left->value) < std::get<char>(right->value); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    type = BOOL;
                    break;
                case LEQ:
                    switch (type) {
                        case INT: value = std::get<int>(left->value) <= std::get<int>(right->value); break;
                        case FLOAT: value = std::get<float>(left->value) <= std::get<float>(right->value); break;
                        case STRING: value = std::get<std::string>(left->value) <= std::get<std::string>(right->value); break;
                        case CHAR: value = std::get<char>(left->value) <= std::get<char>(right->value); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    type = BOOL;
                    break;
                case GT:
                    switch (type) {
                        case INT: value = std::get<int>(left->value) > std::get<int>(right->value); break;
                        case FLOAT: value = std::get<float>(left->value) > std::get<float>(right->value); break;
                        case STRING: value = std::get<std::string>(left->value) > std::get<std::string>(right->value); break;
                        case CHAR: value = std::get<char>(left->value) > std::get<char>(right->value); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    type = BOOL;
                    break;
                case GEQ:
                    switch (type) {
                        case INT: value = std::get<int>(left->value) >= std::get<int>(right->value); break;
                        case FLOAT: value = std::get<float>(left->value) >= std::get<float>(right->value); break;
                        case STRING: value = std::get<std::string>(left->value) >= std::get<std::string>(right->value); break;
                        case CHAR: value = std::get<char>(left->value) >= std::get<char>(right->value); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    type = BOOL;
                    break;
                case EQ:
                    value = left->value == right->value;
                    type = BOOL;
                case NEQ:
                    value = left->value != right->value;
                    type = BOOL;
                default: /* unreachable */;
            }
            // good
            return;
        }
        if (Operation::expressionOperator(op)) {
            switch (op) {
                case PLUS:
                    switch (type) {
                        case INT: value = std::get<int>(left->value) + std::get<int>(right->value); break;
                        case FLOAT: value = std::get<float>(left->value) + std::get<float>(right->value); break;
                        case STRING: value = std::get<std::string>(left->value) + std::get<std::string>(right->value); break;
                        case CHAR: value = std::get<char>(left->value) + std::get<char>(right->value); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case MINUS:
                    switch (type) {
                        case INT: value = std::get<int>(left->value) - std::get<int>(right->value); break;
                        case FLOAT: value = std::get<float>(left->value) - std::get<float>(right->value); break;
                        case CHAR: value = std::get<char>(left->value) - std::get<char>(right->value); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case MULT:
                    switch (type) {
                        case INT: value = std::get<int>(left->value) * std::get<int>(right->value); break;
                        case FLOAT: value = std::get<float>(left->value) * std::get<float>(right->value); break;
                        case CHAR: value = std::get<char>(left->value) * std::get<char>(right->value); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case DIV:
                    switch (type) {
                        case INT: value = std::get<int>(left->value) / std::get<int>(right->value); break;
                        case FLOAT: value = std::get<float>(left->value) / std::get<float>(right->value); break;
                        case CHAR: value = std::get<char>(left->value) / std::get<char>(right->value); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case POW:
                    switch (type) {
                        case INT: value = static_cast<int>(std::pow(std::get<int>(left->value), std::get<int>(right->value))); break;
                        case FLOAT: value = static_cast<float>(std::pow(std::get<float>(left->value), std::get<float>(right->value))); break;
                        case CHAR: value = static_cast<char>(std::pow(std::get<char>(left->value), std::get<char>(right->value))); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                default:;
                    throw std::runtime_error("Invalid expression operator");
            }
        }
    } 
}


std::string AST::typeStr() const {
    return TypeNms::typeToStr(type);
}

std::string AST::valueStr() const {
    using namespace TypeNms;
    switch(type) {
        case INT :
            return std::to_string(std::get<int>(value));
        case FLOAT:
            return std::to_string(std::get<float>(value));
        case BOOL:
            return std::to_string(std::get<bool>(value));
        case CHAR:
            return std::to_string(std::get<char>(value));
        case STRING:
            return std::get<std::string>(value);
        default:
            throw std::invalid_argument("cred ca doar astea se poate");
    }
}