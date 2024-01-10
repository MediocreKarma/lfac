#include "Utils.h"

char* Utils::preprocessStringLiteral(const char* input) {
    if (input == NULL) {
        return NULL;
    }
    size_t inputLength = 0;
    while (input[inputLength] != '\0') {
        inputLength++;
    }
    char* result = (char*)malloc((2 * inputLength + 2) * sizeof(char));
    if (result == NULL) {
        return NULL;
    }
    size_t resultIndex = 0;
    char prevChar = input[0];
    for (size_t i = 1; i < inputLength - 1; i++) {
        if (prevChar != '\\') {
            if (input[i] != '\\') {
                result[resultIndex++] = input[i];
            }
            prevChar = input[i];
            continue;
        }
        switch(input[i]) {
            case '"':
                result[resultIndex++] = '"'; // double quote
                break;
            case '0':
                result[resultIndex++] = '\0'; // null character
                break;
            case 'n':
                result[resultIndex++] = '\n'; // newline
                break;
            case 't':
                result[resultIndex++] = '\t'; // tab
                break;
            case 'r':
                result[resultIndex++] = '\r'; // carriage return
                break;
            case '\'':
                result[resultIndex++] = '\''; // single quote
                break;
            case '\\':
                result[resultIndex++] = '\\'; // backslash
                break;
            case 'a':
                result[resultIndex++] = '\a'; // alert (bell)
                break;
            case 'b':
                result[resultIndex++] = '\b'; // backspace
                break;
            case 'f':
                result[resultIndex++] = '\f'; // form feed
                break;
            case 'v':
                result[resultIndex++] = '\v'; // vertical tab
                break;
            default:
                result[resultIndex++] = prevChar;
                result[resultIndex++] = input[i];
                break;
        }
        prevChar = input[i];
    }
    result[resultIndex] = '\0';
    return result;
}

char Utils::preprocessCharLiteral(const char* input) {
    char* processed = preprocessStringLiteral(input);
    if (processed == NULL or strlen(processed) < 1) {
        return '\0';
    }

    char desired = processed[0];
    free(processed);
    return desired;
}

std::string Utils::encodeStringValue(const std::string& input) {
    std::string result;
    for (char c : input) {
        switch (c) {
            case '"':
                result += "\\\"";
                break;
            case '\0':
                result += "\\0";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\t':
                result += "\\t";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\'':
                result += "\\'";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\a':
                result += "\\a";
                break;
            case '\b':
                result += "\\b";
                break;
            case '\f':
                result += "\\f";
                break;
            case '\v':
                result += "\\v";
                break;
            default:
                result += c;
                break;
        }
    }
    return result;
}

void Utils::printWarning(const std::string& input, bool lineno) {
    std::cerr << "\033[1;33mWarning:\033[0m " + input + " " + (!lineno ? "" : "\033[1;36m(at line: " + std::to_string(yylineno)+ ")") + "\033[0m\n";
}

void Utils::printError(const std::string& input, bool lineno) {
    std::cerr << "\033[1;31mError:\033[0m " + input + " " + (!lineno ? "" : "\033[1;36m(at line: " + std::to_string(yylineno)+ ")") + "\033[0m\n";
}