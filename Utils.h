#ifndef _UTILS__
#define _UTILS__

#include <cstring>
#include <malloc.h>

namespace Utils {
    char* preprocessStringLiteral(const char* input);
    char  preprocessCharLiteral(const char* input);
}

#endif