#include "AST.h"

AST::AST(Operation::BinaryOp op, const AST& _left, const AST& _right) :
    type(), value(), left(&_left), right(&_right) {

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
        // disaster?
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
                        default:; // yyerror
                    }
                    break;
                case LEQ:
                    switch (type) {
                        case INT: value = std::get<int>(left->value) <= std::get<int>(right->value); break;
                        case FLOAT: value = std::get<float>(left->value) <= std::get<float>(right->value); break;
                        case STRING: value = std::get<std::string>(left->value) <= std::get<std::string>(right->value); break;
                        case CHAR: value = std::get<char>(left->value) <= std::get<char>(right->value); break;
                        default:; // yyerror
                    }
                    break;
                case GT:
                    switch (type) {
                        case INT: value = std::get<int>(left->value) > std::get<int>(right->value); break;
                        case FLOAT: value = std::get<float>(left->value) > std::get<float>(right->value); break;
                        case STRING: value = std::get<std::string>(left->value) > std::get<std::string>(right->value); break;
                        case CHAR: value = std::get<char>(left->value) > std::get<char>(right->value); break;
                        default:; // yyerror
                    }
                    break;
                case GEQ:
                    switch (type) {
                        case INT: value = std::get<int>(left->value) >= std::get<int>(right->value); break;
                        case FLOAT: value = std::get<float>(left->value) >= std::get<float>(right->value); break;
                        case STRING: value = std::get<std::string>(left->value) >= std::get<std::string>(right->value); break;
                        case CHAR: value = std::get<char>(left->value) >= std::get<char>(right->value); break;
                        default:; // yyerror
                    }
                    break;
                case EQ:
                    value = left->value == right->value;
                case NEQ:
                    value = left->value != right->value;
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
                        default:; // yyerror
                    }
                    break;
                case MINUS:
                    switch (type) {
                        case INT: value = std::get<int>(left->value) - std::get<int>(right->value); break;
                        case FLOAT: value = std::get<float>(left->value) - std::get<float>(right->value); break;
                        case CHAR: value = std::get<char>(left->value) - std::get<char>(right->value); break;
                        default:; // yyerror
                    }
                    break;
                case MULT:
                    switch (type) {
                        case INT: value = std::get<int>(left->value) * std::get<int>(right->value); break;
                        case FLOAT: value = std::get<float>(left->value) * std::get<float>(right->value); break;
                        case CHAR: value = std::get<char>(left->value) * std::get<char>(right->value); break;
                        default:; // yyerror
                    }
                    break;
                case DIV:
                    switch (type) {
                        case INT: value = std::get<int>(left->value) / std::get<int>(right->value); break;
                        case FLOAT: value = std::get<float>(left->value) / std::get<float>(right->value); break;
                        case CHAR: value = std::get<char>(left->value) / std::get<char>(right->value); break;
                        default:; // yyerror
                    }
                    break;
                case POW:
                    switch (type) {
                        case INT: value = static_cast<int>(std::pow(std::get<int>(left->value), std::get<int>(right->value))); break;
                        case FLOAT: value = static_cast<float>(std::pow(std::get<float>(left->value), std::get<float>(right->value))); break;
                        case CHAR: value = static_cast<char>(std::pow(std::get<char>(left->value), std::get<char>(right->value))); break;
                        default:; // yyerror
                    }
                    break;
                default:;
                    // yyerror
            }
        }
    } 
}

/*
try { result = get<int>(var1) + get<int>(var2); } catch(...) {}
try { result = get<float>(var1) + get<float>(var2); } catch(...) {}
try { result = get<string>(var1) + get<string>(var2); } catch(...) {}
try { result = get<char>(var1) + get<char>(var2); } catch(...) {}

try { result = get<int>(var1) + get<int>(var2); } 
catch(...) {
    try { result = get<float>(var1) + get<float>(var2); } 
    catch(...) {
        try { result = get<string>(var1) + get<string>(var2); } 
        catch(...) {
            try { result = get<char>(var1) + get<char>(var2); } 
            catch(...) {
                yyerror();
            }
        }
    }
}



*/