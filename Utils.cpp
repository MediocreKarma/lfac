#include "Utils.h"

char* Utils::preprocessStringLiteral(const char* input) {
    // acum as putea clar sa le fac std::string dar mi e sila
    // daca ti trebe n alta parte sau le muti din lex le folosesti pe astea si dai si tu free lol
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