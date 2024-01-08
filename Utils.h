#pragma once

extern int yylineno; // ca sa putem printa warninguri care-mi spun si linia. Nu le putem face exceptii pt ca nu sunt exceptii

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
