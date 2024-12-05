#include "ctype.h"

bool isspace(char c)
{
    return (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v' ||
            c == '\b');
}

