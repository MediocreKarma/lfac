#pragma once

extern int yylineno;

#include <string>
#include <iostream>

#include <cstring>
#include <malloc.h>

namespace Utils {
    char* preprocessStringLiteral(const char* input);
    char  preprocessCharLiteral(const char* input);
    std::string encodeStringValue(const std::string& input);

    void printError(const std::string& input);
    void printWarning(const std::string& input);
}
