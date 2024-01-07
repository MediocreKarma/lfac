#include "AST.h"

AST::~AST() {
    delete _left;
    delete _right;
}

AST::AST(const AST* other) {
    using namespace TypeNms;
    _type = other->_type;
    _value = other->_value;
    _left = other;
}

AST::AST(int literal) {
    using namespace TypeNms;
    _type = INT;
    _value = literal;
}

AST::AST(float literal) {
    using namespace TypeNms;
    _type = FLOAT;
    _value = (float)literal;
}

AST::AST(bool literal) {
    using namespace TypeNms;
    _type = FLOAT;
    _value = literal;
}

AST::AST(const char* literal) {
    using namespace TypeNms;
    _type = STRING;
    _value = std::string(literal);
}

AST::AST(char literal) {
    using namespace TypeNms;
    _type = CHAR;
    _value = literal;
}

AST::AST(const SymbolData& symbol) {
    _type = symbol.type();
    // ar trebui sa putem hold-ui gen. toti membrii aici, in caz de AST-ul e doar identifierul. nu?
    // _value = symbol.containedSymbols(); sau ceva de genul
}

AST::AST(Operation::UnaryOp op, AST*& _left) :
    _type(), _value(), _left(_left) {
    
    using namespace TypeNms;
    using enum Operation::UnaryOp;

    _type = _left->type();
    if (Operation::booleanOperator(op)) {
        switch (op) {
            case NEGB:
                if (!std::holds_alternative<bool>(_left->_value)) {
                    throw std::invalid_argument("Cannot apply negation operator to a non-boolean value");
                }
                _value = !std::get<bool>(_left->_value);
        }
    }
    else if (Operation::expressionOperator(op)) {
        switch (op) {
            case NEG:
                switch(type()) {
                    case INT:
                        _value = -std::get<int>(_left->_value);
                        break;
                    case FLOAT:
                        _value = -std::get<float>(_left->_value);
                        break;
                    case CHAR:
                        _value = -std::get<char>(_left->_value);
                        break;
                    default:
                        throw std::invalid_argument("Cannot apply unary minus operator to a non-numeric value");
                } 

        }
    }
}

AST::AST(Operation::BinaryOp op, AST*& _left, AST*& _right) :
    _type(), _value(), _left(_left), _right(_right) {

    // todo: caz special pt minus ceva
    if (_left == nullptr or _right == nullptr) {
        throw std::runtime_error("Cannot construct binary-op AST with null values");
    }

    using namespace TypeNms;
    using enum Operation::BinaryOp;

    // no type casting
    if (_left->_type != _right->_type) {
        throw std::runtime_error(std::string(
            "Binary operation using " +
            typeToStr(_left->_type) + 
            " and " +
            typeToStr(_right->_type) +
            " is not allowed!"
        ).c_str());
    }
    if (_left->_value.index() != _right->_value.index()) {
        throw std::runtime_error("std::variant indexes not matching in AST value index evaluation");
    }
    _type = _left->_type;
    if (_type == BOOL) {
        if (Operation::booleanOperator(op)) {
            switch (op) {
                case ORB:
                    _value = std::get<bool>(_left->_value) || std::get<bool>(_right->_value);
                    break;
                case ANDB:
                    _value = std::get<bool>(_left->_value) && std::get<bool>(_right->_value);
                    break;
                case EQ:
                    _value = std::get<bool>(_left->_value) == std::get<bool>(_right->_value);
                    break;
                case NEQ:
                    _value = std::get<bool>(_left->_value) != std::get<bool>(_right->_value);
                    break;
                default: throw std::runtime_error("Invalid boolean operator");;
            }
            // good
            return;
        }
        throw std::runtime_error("Invalid operation for boolean type");
    }
    else {
        if (Operation::conversionOperator(op)) {
            switch (op) {
                case LT:
                    switch (_type) {
                        case INT: _value = std::get<int>(_left->_value) < std::get<int>(_right->_value); break;
                        case FLOAT: _value = std::get<float>(_left->_value) < std::get<float>(_right->_value); break;
                        case STRING: _value = std::get<std::string>(_left->_value) < std::get<std::string>(_right->_value); break;
                        case CHAR: _value = std::get<char>(_left->_value) < std::get<char>(_right->_value); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    _type = BOOL;
                    break;
                case LEQ:
                    switch (_type) {
                        case INT: _value = std::get<int>(_left->_value) <= std::get<int>(_right->_value); break;
                        case FLOAT: _value = std::get<float>(_left->_value) <= std::get<float>(_right->_value); break;
                        case STRING: _value = std::get<std::string>(_left->_value) <= std::get<std::string>(_right->_value); break;
                        case CHAR: _value = std::get<char>(_left->_value) <= std::get<char>(_right->_value); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    _type = BOOL;
                    break;
                case GT:
                    switch (_type) {
                        case INT: _value = std::get<int>(_left->_value) > std::get<int>(_right->_value); break;
                        case FLOAT: _value = std::get<float>(_left->_value) > std::get<float>(_right->_value); break;
                        case STRING: _value = std::get<std::string>(_left->_value) > std::get<std::string>(_right->_value); break;
                        case CHAR: _value = std::get<char>(_left->_value) > std::get<char>(_right->_value); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    _type = BOOL;
                    break;
                case GEQ:
                    switch (_type) {
                        case INT: _value = std::get<int>(_left->_value) >= std::get<int>(_right->_value); break;
                        case FLOAT: _value = std::get<float>(_left->_value) >= std::get<float>(_right->_value); break;
                        case STRING: _value = std::get<std::string>(_left->_value) >= std::get<std::string>(_right->_value); break;
                        case CHAR: _value = std::get<char>(_left->_value) >= std::get<char>(_right->_value); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    _type = BOOL;
                    break;
                case EQ:
                    _value = _left->_value == _right->_value;
                    _type = BOOL;
                    break;
                case NEQ:
                    _value = _left->_value != _right->_value;
                    _type = BOOL;
                    break;
                default: throw std::runtime_error("Invalid conversion operator");
            }
            // good
            return;
        }
        if (Operation::expressionOperator(op)) {
            switch (op) {
                case PLUS:
                    switch (_type) {
                        case INT: _value = std::get<int>(_left->_value) + std::get<int>(_right->_value); break;
                        case FLOAT: _value = std::get<float>(_left->_value) + std::get<float>(_right->_value); break;
                        case STRING: _value = std::get<std::string>(_left->_value) + std::get<std::string>(_right->_value); break;
                        case CHAR: _value = std::get<char>(_left->_value) + std::get<char>(_right->_value); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case MINUS:
                    switch (_type) {
                        case INT: _value = std::get<int>(_left->_value) - std::get<int>(_right->_value); break;
                        case FLOAT: _value = std::get<float>(_left->_value) - std::get<float>(_right->_value); break;
                        case CHAR: _value = std::get<char>(_left->_value) - std::get<char>(_right->_value); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case MULT:
                    switch (_type) {
                        case INT: _value = std::get<int>(_left->_value) * std::get<int>(_right->_value); break;
                        case FLOAT: _value = std::get<float>(_left->_value) * std::get<float>(_right->_value); break;
                        case CHAR: _value = std::get<char>(_left->_value) * std::get<char>(_right->_value); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case DIV:
                    switch (_type) {
                        case INT: _value = std::get<int>(_left->_value) / std::get<int>(_right->_value); break;
                        case FLOAT: _value = std::get<float>(_left->_value) / std::get<float>(_right->_value); break;
                        case CHAR: _value = std::get<char>(_left->_value) / std::get<char>(_right->_value); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case POW:
                    switch (_type) {
                        case INT: _value = static_cast<int>(std::pow(std::get<int>(_left->_value), std::get<int>(_right->_value))); break;
                        case FLOAT: _value = static_cast<float>(std::pow(std::get<float>(_left->_value), std::get<float>(_right->_value))); break;
                        case CHAR: _value = static_cast<char>(std::pow(std::get<char>(_left->_value), std::get<char>(_right->_value))); break;
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
    using namespace TypeNms;
    if (_type == CUSTOM) {
        return "Custom type";
    }
    return TypeNms::typeToStr(_type);
}

std::string AST::valueStr() const {
    using namespace TypeNms;
    switch(_type) {
        case INT :
            return std::to_string(std::get<int>(_value));
        case FLOAT:
            return std::to_string(std::get<float>(_value));
        case BOOL:
            return (std::get<bool>(_value) == 1) ? "true" : "false";
        case CHAR:
            return std::to_string(std::get<char>(_value));
        case STRING:
            return std::get<std::string>(_value);
        default:
            return "Non-base-type (unreadable value)";
    }
}