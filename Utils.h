#pragma once

#include <cstring>
#include <malloc.h>

namespace Utils {
    char* preprocessStringLiteral(const char* input);
    char  preprocessCharLiteral(const char* input);
}
