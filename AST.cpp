#include "AST.h"

AST::~AST() {
    delete _left;
    delete _right;
}

AST::AST(int literal) {
    using namespace TypeNms;
    _symbol = SymbolData("", "", INT, SymbolData::Variable);
    _symbol.assign(literal);
    _type = _symbol.type();
}

AST::AST(float literal) {
    using namespace TypeNms;
    _symbol = SymbolData("", "", FLOAT, SymbolData::Variable);
    _symbol.assign(literal);
    _type = _symbol.type();
}

AST::AST(bool literal) {
    using namespace TypeNms;
    _symbol = SymbolData("", "", BOOL, SymbolData::Variable);
    _symbol.assign(literal);
    _type = _symbol.type();
}

AST::AST(const char* literal) {
    using namespace TypeNms;
    _symbol = SymbolData("", "", STRING, SymbolData::Variable);
    _symbol.assign(std::string(literal));
    _type = _symbol.type();
}

AST::AST(char literal) {
    using namespace TypeNms;
    _symbol = SymbolData("", "", CHAR, SymbolData::Variable);
    _symbol.assign(literal);
    _type = _symbol.type();
}

AST::AST(const SymbolData& symbol) {
    _symbol = symbol;
    _type = _symbol.type();
}

TypeNms::Type AST::type() const {
    return _type;
}

SymbolData AST::evaluateUnary() const {
    using namespace TypeNms;
    using enum Operation::UnaryOp;

    auto symbol = SymbolData();
    auto op = static_cast<Operation::UnaryOp>(_op);

    // evaluam doar left
    auto leftEval = _left->evaluate();

    symbol.setType(leftEval.type());
    if (Operation::booleanOperator(op)) {
        switch (op) {
            case NEGB:
                if (!std::holds_alternative<bool>(leftEval.value())) {
                    throw std::invalid_argument("Cannot apply negation operator to a non-boolean value");
                }
                symbol.assign(!std::get<bool>(leftEval.value()));
        }
    }
    else if (Operation::expressionOperator(op)) {
        switch (op) {
            case NEG:
                switch(symbol.type()) {
                    case INT:
                        symbol.assign(-std::get<int>(leftEval.value()));
                        break;
                    case FLOAT:
                        symbol.assign(-std::get<float>(leftEval.value()));
                        break;
                    case CHAR:
                        symbol.assign(-std::get<char>(leftEval.value()));
                        break;
                    default:
                        throw std::invalid_argument("Cannot apply unary minus operator to a non-numeric or char value");
                } 
        }
    }
    return symbol;
}

AST::AST(Operation::UnaryOp op, const AST* _left) :
    _symbol(), _left(_left) {
    
    _isOperation = true;
    _op = static_cast<int>(op);
    _operationType = Operation::Type::UNARY;
}

SymbolData AST::evaluateBinary() const {
    using namespace TypeNms;
    using enum Operation::BinaryOp;

    if (_left == nullptr or _right == nullptr) {
        throw std::runtime_error("Cannot evaluate binary-op AST with null values on either side");
    }

    auto symbol = SymbolData();
    auto op = static_cast<Operation::BinaryOp>(_op);

    auto leftEval = _left->evaluate();
    auto rightEval = _right->evaluate();

   
    if (leftEval.value().index() != rightEval.value().index()) {
        throw std::runtime_error("std::variant indexes not matching in AST value index evaluation");
    }
    symbol.setType(leftEval.type());
    if (symbol.type() == BOOL) {
        if (Operation::booleanOperator(op)) {
            switch (op) {
                case ORB:
                    symbol.assign(std::get<bool>(leftEval.value()) || std::get<bool>(rightEval.value()));
                    break;
                case ANDB:
                    symbol.assign(std::get<bool>(leftEval.value()) && std::get<bool>(rightEval.value()));
                    break;
                case EQ:
                    symbol.assign(std::get<bool>(leftEval.value()) == std::get<bool>(rightEval.value()));
                    break;
                case NEQ:
                    symbol.assign(std::get<bool>(leftEval.value()) != std::get<bool>(rightEval.value()));
                    break;
                default: throw std::runtime_error("Invalid boolean operator");;
            }
            
        }
        throw std::runtime_error("Invalid operation for boolean type");
    }
    else {
        if (Operation::conversionOperator(op)) {
            auto oldType = symbol.type();
            symbol.setType(BOOL);
            switch (op) {
                case LT:
                    switch (oldType) {
                        case INT: symbol.assign(std::get<int>(leftEval.value()) < std::get<int>(rightEval.value())); break;
                        case FLOAT: symbol.assign(std::get<float>(leftEval.value()) < std::get<float>(rightEval.value())); break;
                        case STRING: symbol.assign(std::get<std::string>(leftEval.value()) < std::get<std::string>(rightEval.value())); break;
                        case CHAR: symbol.assign(std::get<char>(leftEval.value()) < std::get<char>(rightEval.value())); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case LEQ:
                    switch (oldType) {
                        case INT: symbol.assign(std::get<int>(leftEval.value()) <= std::get<int>(rightEval.value())); break;
                        case FLOAT: symbol.assign(std::get<float>(leftEval.value()) <= std::get<float>(rightEval.value())); break;
                        case STRING: symbol.assign(std::get<std::string>(leftEval.value()) <= std::get<std::string>(rightEval.value())); break;
                        case CHAR: symbol.assign(std::get<char>(leftEval.value()) <= std::get<char>(rightEval.value())); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case GT:
                    switch (oldType) {
                        case INT: symbol.assign(std::get<int>(leftEval.value()) > std::get<int>(rightEval.value())); break;
                        case FLOAT: symbol.assign(std::get<float>(leftEval.value()) > std::get<float>(rightEval.value())); break;
                        case STRING: symbol.assign(std::get<std::string>(leftEval.value()) > std::get<std::string>(rightEval.value())); break;
                        case CHAR: symbol.assign(std::get<char>(leftEval.value()) > std::get<char>(rightEval.value())); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case GEQ:
                    switch (oldType) {
                        case INT: symbol.assign(std::get<int>(leftEval.value()) >= std::get<int>(rightEval.value())); break;
                        case FLOAT: symbol.assign(std::get<float>(leftEval.value()) >= std::get<float>(rightEval.value())); break;
                        case STRING: symbol.assign(std::get<std::string>(leftEval.value()) >= std::get<std::string>(rightEval.value())); break;
                        case CHAR: symbol.assign(std::get<char>(leftEval.value()) >= std::get<char>(rightEval.value())); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case EQ:
                    symbol.assign(leftEval.value() == rightEval.value());
                    break;
                case NEQ:
                    symbol.assign(leftEval.value() != rightEval.value());
                    break;
                default: throw std::runtime_error("Invalid conversion operator");
            }
        }
        if (Operation::expressionOperator(op)) {
            switch (op) {
                case PLUS:
                    switch (symbol.type()) {
                        case INT: symbol.assign(std::get<int>(leftEval.value()) + std::get<int>(rightEval.value())); break;
                        case FLOAT: symbol.assign(std::get<float>(leftEval.value()) + std::get<float>(rightEval.value())); break;
                        case STRING: symbol.assign(std::get<std::string>(leftEval.value()) + std::get<std::string>(rightEval.value())); break;
                        case CHAR: symbol.assign(std::get<char>(leftEval.value()) + std::get<char>(rightEval.value())); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case MINUS:
                    switch (symbol.type()) {
                        case INT: symbol.assign(std::get<int>(leftEval.value()) - std::get<int>(rightEval.value())); break;
                        case FLOAT: symbol.assign(std::get<float>(leftEval.value()) - std::get<float>(rightEval.value())); break;
                        case CHAR: symbol.assign(std::get<char>(leftEval.value()) - std::get<char>(rightEval.value())); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case MULT:
                    switch (symbol.type()) {
                        case INT: symbol.assign(std::get<int>(leftEval.value()) * std::get<int>(rightEval.value())); break;
                        case FLOAT: symbol.assign(std::get<float>(leftEval.value()) * std::get<float>(rightEval.value())); break;
                        case CHAR: symbol.assign(std::get<char>(leftEval.value()) * std::get<char>(rightEval.value())); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case DIV:
                    switch (symbol.type()) {
                        case INT: symbol.assign(std::get<int>(leftEval.value()) / std::get<int>(rightEval.value())); break;
                        case FLOAT: symbol.assign(std::get<float>(leftEval.value()) / std::get<float>(rightEval.value())); break;
                        case CHAR: symbol.assign(std::get<char>(leftEval.value()) / std::get<char>(rightEval.value())); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case POW:
                    switch (symbol.type()) {
                        case INT: symbol.assign(static_cast<int>(std::pow(std::get<int>(leftEval.value()), std::get<int>(rightEval.value())))); break;
                        case FLOAT: symbol.assign(static_cast<float>(std::pow(std::get<float>(leftEval.value()), std::get<float>(rightEval.value())))); break;
                        case CHAR: symbol.assign(static_cast<char>(std::pow(std::get<char>(leftEval.value()), std::get<char>(rightEval.value())))); break;
                        default: throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                default:;
                    throw std::runtime_error("Invalid expression operator");
            }
        }
    } 

    return symbol;
}

AST::AST(Operation::BinaryOp op, const AST* left, const AST* right) :
    _left(left), _right(right) {

    using namespace TypeNms;
    using enum Operation::BinaryOp;

    _isOperation = true;
    _op = static_cast<int>(op);
    _operationType = Operation::Type::BINARY;

    // set type accordingly
    if (_left->_type != _right->_type) {
        throw std::runtime_error(std::string(
            "Binary operation using " +
            typeToStr(_left->_type) + 
            " and " +
            typeToStr(_right->_type) +
            " is not allowed!"
        ).c_str());
    }
    _type = _left->_type;
    if (_type == BOOL) { 
        if (!Operation::booleanOperator(op)) {
            throw std::runtime_error("Invalid operation for boolean type");
        }
    }
    else {
        if (Operation::conversionOperator(op)) {
            auto oldType = _type;
            _type = BOOL;
            switch (op) {
                case LT:
                    if (oldType == CUSTOM or oldType == BOOL) {
                        throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case LEQ:
                    if (oldType == CUSTOM or oldType == BOOL) {
                        throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case GT:
                    if (oldType == CUSTOM or oldType == BOOL) {
                        throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case GEQ:
                    if (oldType == CUSTOM or oldType == BOOL) {
                        throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case EQ:
                    break;
                case NEQ:
                    break;
                default: throw std::runtime_error("Invalid conversion operator");
            }
        }
        if (Operation::expressionOperator(op)) {
            switch (op) {
                case PLUS:
                    if (_type == CUSTOM or _type == BOOL) {
                        throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case MINUS:
                    if (_type == CUSTOM or _type == BOOL or _type == STRING) {
                        throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case MULT:
                    if (_type == CUSTOM or _type == BOOL or _type == STRING) {
                        throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case DIV:
                    if (_type == CUSTOM or _type == BOOL or _type == STRING) {
                        throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
                    }
                    break;
                case POW:
                    if (_type == CUSTOM or _type == BOOL or _type == STRING) {
                        throw std::runtime_error("Invalid operand types for binary operator"); // yyerror
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
    return evaluate().valueStr();
}

SymbolData AST::symbol() const {
    return evaluate();
}

SymbolData AST::evaluate() const {
    using enum Operation::Type;

    if (!_isOperation) {
        // return value of currently held symbol
        return _symbol;
    }
    else {
        switch(_operationType) {
            case UNARY :
                return evaluateUnary();
            case BINARY:
                return evaluateBinary();
            default: throw "Unavailable operation type";
        }
    }
}