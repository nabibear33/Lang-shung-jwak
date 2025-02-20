#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <functional>
#include <vector>
#include "error.h"
#include "token.h"

using TokenCheckFunc = std::function<bool(std::wstring)>;
using TokenValueFunc = std::function<int(std::wstring)>;

typedef struct
{
    TokenType type;
    TokenCheckFunc checkFunc;
    TokenValueFunc valueFunc;
}TokenCheck;

typedef std::vector<TokenCheck> TokenCheckList;

TokenValueFunc getNoneValueFunction();
TokenValueFunc getKekekeValueFunc();
TokenValueFunc getLengthValueFunc();

TokenCheckFunc createTokenCheckFuncByString(std::vector<std::wstring> tokenStringList, wchar_t stop);
TokenCheckFunc createTokenCheckFuncBySeq(wchar_t one, std::wstring twoOver);
TokenCheckFunc getPrintTokenCheckFunc(bool isNumberPrint);
TokenCheckFunc getInputTokenCheckFunc();

class Lexer
{
private:
    TokenCheckList tokenCheckList;
    inline bool isWhiteSpace(wchar_t c);
    inline bool isCodeChar(wchar_t c);

public:
    Lexer();

    TokenList lexing(std::wstring code);
    wchar_t nextChar(std::wstring code, int idx);
    Token getToken(std::wstring buffer);
};

#endif
