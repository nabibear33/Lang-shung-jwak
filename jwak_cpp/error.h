#ifndef ERROR_H
#define ERROR_H

#include <string>

enum class ErrorType
{
    SYNTAX,
    RUNTIME,
    ZERO_DIVISION
};

typedef struct
{
    ErrorType type;
    std::wstring message;
}Error;

void printError(Error error);

#endif
