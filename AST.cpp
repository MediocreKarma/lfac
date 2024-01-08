#include "AST.h"

AST::~AST() {
    delete _left;
    delete _right;
}

// AST::AST(const SymbolData* other) {
//     using namespace TypeNms;
//     symbol = other->symbol;
//     symbol._symbol.setType(other->symbol.type());
//     _value._symbol.assign(other->_symbol.value();
//     _left = other;
// }

AST::AST(const AST* other) {
    using namespace TypeNms;
    _symbol = SymbolData(other->_symbol);
    // sper ca e suficient
    _left = other;
}

AST::AST(int literal) {
    using namespace TypeNms;
    _symbol = SymbolData("", "", INT, SymbolData::Variable);
    _symbol.assign(literal);
}

AST::AST(float literal) {
    using namespace TypeNms;
    _symbol = SymbolData("", "", FLOAT, SymbolData::Variable);
    _symbol.assign(literal);
}

AST::AST(bool literal) {
    using namespace TypeNms;
    _symbol = SymbolData("", "", BOOL, SymbolData::Variable);
    _symbol.assign(literal);
}

AST::AST(const char* literal) {
    using namespace TypeNms;
    _symbol = SymbolData("", "", STRING, SymbolData::Variable);
    _symbol.assign(std::string(literal));
}

AST::AST(char literal) {
    using namespace TypeNms;
    _symbol = SymbolData("", "", CHAR, SymbolData::Variable);
    _symbol.assign(literal);
}

AST::AST(const SymbolData& symbol) {
    _symbol = symbol;
}

AST::AST(Operation::UnaryOp op, const AST* _left) :
    _symbol(), _left(_left) {
    
    using namespace TypeNms;
    using enum Operation::UnaryOp;

    _symbol.setType(_left->_symbol.type());
    if (Operation::booleanOperator(op)) {
        switch (op) {
            case NEGB:
                if (!std::holds_alternative<bool>(_left->_symbol.value())) {
                    throw std::invalid_argument("Cannot apply negation operator to a non-boolean value");
                }
                _symbol.assign(!std::get<bool>(_left->_symbol.value()));
        }
    }
    else if (Operation::expressionOperator(op)) {
        switch (op) {
            case NEG:
                switch(_symbol.type()) {
                    case INT:
                        _symbol.assign(-std::get<int>(_left->_symbol.value()));
                        break;
                    case FLOAT:
                        _symbol.assign(-std::get<float>(_left->_symbol.value()));
                        break;
                    case CHAR:
                        _symbol.assign(-std::get<char>(_left->_symbol.value()));
                        break;
                    default:
                        throw std::invalid_argument("Cannot apply unary minus operator to a non-numeric value");
                } 

        }
    }
}

AST::AST(Operation::BinaryOp op, const AST* left, const AST* right) :
    _left(left), _right(right) {

    if (_left == nullptr or _right == nullptr) {
        throw std::runtime_error("Cannot construct binary-op AST with null values");
    }

    using namespace TypeNms;
    using enum Operation::BinaryOp;

    // no type casting
    if (_left->_symbol.type() != _right->_symbol.type()) {
        throw std::runtime_error(std::string(
            "Binary operation using " +
            typeToStr(_left->_symbol.type()) + 
            " and " +
            typeToStr(_right->_symbol.type()) +
            " is not allowed!"
        ).c_str());
    }
    if (_left->_symbol.value().index() != _right->_symbol.value().index()) {
        throw std::runtime_error("std::variant indexes not matching in AST value index evaluation");
    }
    _symbol.setType(_left->_symbol.type());
    if (_symbol.type() == BOOL) {
        // pentru astea e ok ca da tot bool
        if (Operation::booleanOperator(op)) {
            switch (op) {
                case ORB:
                    _symbol.assign(std::get<bool>(_left->_symbol.value()) || std::get<bool>(_right->_symbol.value()));
                    break;
                case ANDB:
                    _symbol.assign(std::get<bool>(_left->_symbol.value()) && std::get<bool>(_right->_symbol.value()));
                    break;
                case EQ:
                    _symbol.assign(std::get<bool>(_left->_symbol.value()) == std::get<bool>(_right->_symbol.value()));
                    break;
                case NEQ:
                    _symbol.assign(std::get<bool>(_left->_symbol.value()) != std::get<bool>(_right->_symbol.value()));
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
            // pentru astea tre sa putem asigna bool-uri deci trb oblig schimbat tipu
            auto oldType = _symbol.type();
            _symbol.setType(BOOL); // altfel nu merg asignarile...
            switch (op) {
                case LT:
                    switch (oldType) {
                        case INT: _symbol.assign(std::get<int>(_left->_symbol.value()) < std::get<int>(_right->_symbol.value())); break;
                        case FLOAT: _symbol.assign(std::get<float>(_left->_symbol.value()) < std::get<float>(_right->_symbol.value())); break;
                        case STRING: _symbol.assign(std::get<std::string>(_left->_symbol.value()) < std::get<std::string>(_right->_symbol.value())); break;
                        case CHAR: _symbol.assign(std::get<char>(_left->_symbol.value()) < std::get<char>(_right->_symbol.value())); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case LEQ:
                    switch (oldType) {
                        case INT: _symbol.assign(std::get<int>(_left->_symbol.value()) <= std::get<int>(_right->_symbol.value())); break;
                        case FLOAT: _symbol.assign(std::get<float>(_left->_symbol.value()) <= std::get<float>(_right->_symbol.value())); break;
                        case STRING: _symbol.assign(std::get<std::string>(_left->_symbol.value()) <= std::get<std::string>(_right->_symbol.value())); break;
                        case CHAR: _symbol.assign(std::get<char>(_left->_symbol.value()) <= std::get<char>(_right->_symbol.value())); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case GT:
                    switch (oldType) {
                        case INT: _symbol.assign(std::get<int>(_left->_symbol.value()) > std::get<int>(_right->_symbol.value())); break;
                        case FLOAT: _symbol.assign(std::get<float>(_left->_symbol.value()) > std::get<float>(_right->_symbol.value())); break;
                        case STRING: _symbol.assign(std::get<std::string>(_left->_symbol.value()) > std::get<std::string>(_right->_symbol.value())); break;
                        case CHAR: _symbol.assign(std::get<char>(_left->_symbol.value()) > std::get<char>(_right->_symbol.value())); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case GEQ:
                    switch (oldType) {
                        case INT: _symbol.assign(std::get<int>(_left->_symbol.value()) >= std::get<int>(_right->_symbol.value())); break;
                        case FLOAT: _symbol.assign(std::get<float>(_left->_symbol.value()) >= std::get<float>(_right->_symbol.value())); break;
                        case STRING: _symbol.assign(std::get<std::string>(_left->_symbol.value()) >= std::get<std::string>(_right->_symbol.value())); break;
                        case CHAR: _symbol.assign(std::get<char>(_left->_symbol.value()) >= std::get<char>(_right->_symbol.value())); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case EQ:
                    _symbol.assign(_left->_symbol.value() == _right->_symbol.value());
                    break;
                case NEQ:
                    _symbol.assign(_left->_symbol.value() != _right->_symbol.value());
                    break;
                default: throw std::runtime_error("Invalid conversion operator");
            }
            // good
            return;
        }
        if (Operation::expressionOperator(op)) {
            // astea cred ca s ok asa
            switch (op) {
                case PLUS:
                    switch (_symbol.type()) {
                        case INT: _symbol.assign(std::get<int>(_left->_symbol.value()) + std::get<int>(_right->_symbol.value())); break;
                        case FLOAT: _symbol.assign(std::get<float>(_left->_symbol.value()) + std::get<float>(_right->_symbol.value())); break;
                        case STRING: _symbol.assign(std::get<std::string>(_left->_symbol.value()) + std::get<std::string>(_right->_symbol.value())); break;
                        case CHAR: _symbol.assign(std::get<char>(_left->_symbol.value()) + std::get<char>(_right->_symbol.value())); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case MINUS:
                    switch (_symbol.type()) {
                        case INT: _symbol.assign(std::get<int>(_left->_symbol.value()) - std::get<int>(_right->_symbol.value())); break;
                        case FLOAT: _symbol.assign(std::get<float>(_left->_symbol.value()) - std::get<float>(_right->_symbol.value())); break;
                        case CHAR: _symbol.assign(std::get<char>(_left->_symbol.value()) - std::get<char>(_right->_symbol.value())); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case MULT:
                    switch (_symbol.type()) {
                        case INT: _symbol.assign(std::get<int>(_left->_symbol.value()) * std::get<int>(_right->_symbol.value())); break;
                        case FLOAT: _symbol.assign(std::get<float>(_left->_symbol.value()) * std::get<float>(_right->_symbol.value())); break;
                        case CHAR: _symbol.assign(std::get<char>(_left->_symbol.value()) * std::get<char>(_right->_symbol.value())); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case DIV:
                    switch (_symbol.type()) {
                        case INT: _symbol.assign(std::get<int>(_left->_symbol.value()) / std::get<int>(_right->_symbol.value())); break;
                        case FLOAT: _symbol.assign(std::get<float>(_left->_symbol.value()) / std::get<float>(_right->_symbol.value())); break;
                        case CHAR: _symbol.assign(std::get<char>(_left->_symbol.value()) / std::get<char>(_right->_symbol.value())); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case POW: // what do you mean pow pt char-uri??? what do you mean div pt char-uri??? de ce exista???????
                    switch (_symbol.type()) {
                        case INT: _symbol.assign(static_cast<int>(std::pow(std::get<int>(_left->_symbol.value()), std::get<int>(_right->_symbol.value())))); break;
                        case FLOAT: _symbol.assign(static_cast<float>(std::pow(std::get<float>(_left->_symbol.value()), std::get<float>(_right->_symbol.value())))); break;
                        case CHAR: _symbol.assign(static_cast<char>(std::pow(std::get<char>(_left->_symbol.value()), std::get<char>(_right->_symbol.value())))); break;
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
    if (_symbol.type() == CUSTOM) {
        return "Custom type";
    }
    return TypeNms::typeToStr(_symbol.type());
}

std::string AST::valueStr() const {
    return _symbol.valueStr();
}

const SymbolData& AST::symbol() const {
    return _symbol;
}
