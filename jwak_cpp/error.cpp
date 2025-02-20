#include "error.h"

#include <iostream>

void printError(Error error)
{
    wprintf(L"\n");
    switch(error.type)
    {
        case ErrorType::SYNTAX:
        {
            wprintf(L"SyntaxError: ");
            break;
        }
        case ErrorType::RUNTIME:
        {
            wprintf(L"RuntimeError: ");
            break;
        }
        case ErrorType::ZERO_DIVISION:
        {
            wprintf(L"ZeroDivisionError: ");
        }
    }
    wprintf(L"%s\n", error.message.c_str());
}
