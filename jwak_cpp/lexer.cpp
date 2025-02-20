#include "lexer.h"

#include <bits/stdc++.h>

TokenValueFunc getZeroValueFunc()
{
    return [](std::wstring buffer) -> int
    {
        return 0;
    };
}

TokenValueFunc getKekekeValueFunc()
{
    return [](std::wstring buffer) -> int
    {
        return std::count(buffer.begin(), buffer.end(), L'ㅋ');
    };
}

TokenValueFunc getLengthValueFunc()
{
    return [](std::wstring buffer) -> int
    {
        return buffer.length();
    };
}

TokenCheckFunc createTokenCheckFuncByString(std::vector<std::wstring> tokenStringList, wchar_t ignore)
{
    return [tokenStringList, ignore](std::wstring buffer) -> bool
    {
        int bufferPointer = 0;
        int bufferLength = buffer.length();
        bool isFirst = true;
        for(std::wstring tokenString : tokenStringList)
        {
            int tokenPointer = 0;

            if(!isFirst)
            {
                while(buffer[bufferPointer] != tokenString[tokenPointer])
                {
                    if(buffer[bufferPointer] != ignore)
                    {
                        return false;
                    }

                    bufferPointer++;

                    if(bufferPointer >= bufferLength)
                    {
                        return false;
                    }
                }
            }
            else
            {
                isFirst = false;
            }

            int tokenLength = tokenString.length();
            for(int i = 0; i < tokenLength; i++)
            {
                if(buffer[bufferPointer] == tokenString[tokenPointer])
                {
                    tokenPointer++;
                }

                bufferPointer++;
            }

            if(tokenPointer != tokenString.length())
            {
                return false;
            }
        }

        return true;
    };
}

TokenCheckFunc createTokenCheckFuncBySeq(wchar_t one, std::wstring twoOver)
{
    return [one, twoOver](std::wstring buffer) -> int
    {
        int bufferLength = buffer.length();

        if(bufferLength == 1)
        {
            return buffer[0] == one;
        }

        if(bufferLength == 2)
        {
            return buffer[0] == twoOver[0] && buffer[1] == twoOver[2];
        }

        if(buffer[0] != twoOver[0])
        {
            return false;
        }

        for(int i = 1; i < bufferLength; i++)
        {
            if(buffer[i] == twoOver[2] && buffer[i - 1] != twoOver[1])
            {
                return false;
            }
            else if(buffer[i] != twoOver[1] && buffer[i] != twoOver[2])
            {
                return false;
            }
        }

        if(buffer[bufferLength - 1] != twoOver[2])
        {
            return false;
        }

        return true;
    };
}

TokenCheckFunc getPrintTokenCheckFunc(bool isNumberPrint)
{
    return [isNumberPrint](std::wstring buffer) -> bool
    {
        int bufferPointer = 0;
        int bufferLength = buffer.length();

        if(buffer[0] != L'비' || buffer[1] != L'비')
        {
            return false;
        }
        bufferPointer += 2;

        while(buffer[bufferPointer] == L'ㅋ')
        {
            bufferPointer++;
        }

        if(isNumberPrint)
        {
            if(buffer[bufferPointer] != L'보' || buffer[bufferPointer + 1] != L'호' || buffer[bufferPointer + 2] != L'막')
            {
                return false;
            }
            bufferPointer += 3;

            while(buffer[bufferPointer] == L'ㅋ')
            {
                bufferPointer++;
            }
        }

        if(buffer[bufferPointer] != L'따')
        {
            return false;
        }
        while(buffer[bufferPointer] == L'따')
        {
            bufferPointer++;
        }

        if(buffer[bufferPointer] != L'잇')
        {
            return false;
        }

        return true;
    };
}

TokenCheckFunc getInputTokenCheckFunc()
{
    return [](std::wstring buffer) -> bool
    {
        int bufferPointer = 0;
        int bufferLength = buffer.length();

        if(buffer[0] != L'순' || buffer[1] != L'수')
        {
            return false;
        }
        bufferPointer += 2;

        while(buffer[bufferPointer] == L'ㅋ')
        {
            bufferPointer++;
        }

        if(buffer[bufferPointer] != L'따')
        {
            return false;
        }
        while(buffer[bufferPointer] == L'따')
        {
            bufferPointer++;
        }

        if(buffer[bufferPointer] != L'잇')
        {
            return false;
        }

        return true;
    };
}

inline bool Lexer::isCodeChar(wchar_t c)
{
    return c == L'교' || c == L'주' || c == L'님' || c == L'슝' ||
            c == L'슈' || c == L'우' || c == L'웅' || c == L'좍' ||
            c == L'좌' || c == L'아' || c == L'악' || c == L'비' ||
            c == L'따' || c == L'잇' || c == L'ㅋ' || c == L'보' ||
            c == L'호' || c == L'막' || c == L'하' || c == L'는' ||
            c == L'재' || c == L'미' || c == L'에' || c == L'잇' ||
            c == L'순' || c == L'수' ||
            c == L'~' || c == L';' || c == L',' || c == L'@';
}

inline bool Lexer::isWhiteSpace(wchar_t c)
{
    return c == L' ' || c == L'!' || c == L'?' || c == L'.';
}

Token Lexer::getToken(std::wstring buffer)
{
    for(TokenCheck check : tokenCheckList)
    {
        if(check.checkFunc(buffer))
        {
            return {
                check.type,
                check.valueFunc(buffer)
            };
        }
    }

    return {TokenType::NONE, 0};
}

wchar_t Lexer::nextChar(std::wstring code, int idx)
{
    idx++;
    while(isWhiteSpace(code[idx]))
    {
        idx++;
    }

    return code[idx];
}

TokenList Lexer::lexing(std::wstring code)
{
    TokenList tokenList;

    int codeLength = code.length();
    int idx = 0;
    wchar_t c;
    std::wstring buffer;
    TokenType prevTokenType = TokenType::NONE;
    for(int i = 0; i < codeLength; i++)
    {
        c = code[i];

        if(this->isCodeChar(c) || c == L'\n')
        {
            bool endLine = c == L'\n';

            if(c != L'\n')
            {
                buffer += c;
            }

            if(c == L'ㅋ' && this->nextChar(code, i) == L'ㅋ')
            {
                continue;
            }

            Token token;
            if(this->nextChar(code, i) != L'ㅋ')
            {
                token = getToken(buffer);
            }

            if(token.type != TokenType::NONE)
            {
                buffer = L"";

                switch(token.type)
                {
                    case TokenType::ADD:
                    case TokenType::SUB:
                    case TokenType::MUL:
                    case TokenType::DIV:
                    {
                        if(token.type != prevTokenType)
                        {
                            tokenList.push_back(token);
                            prevTokenType = token.type;
                        }
                        break;
                    }

                    default:
                    {
                        tokenList.push_back(token);
                        prevTokenType = token.type;
                        break;
                    }
                }
            }

            if(endLine)
            {
                if(!buffer.empty())
                {
                    throw Error{ErrorType::SYNTAX, std::wstring(L"어떻게 이게 리슝좍이냐!")};
                }
                tokenList.push_back(Token{TokenType::END_LINE, 0});
                prevTokenType = TokenType::END_LINE;
            }
        }
        else if(!this->isWhiteSpace(c))
        {
            // 코드에 사용되는 문자도, 공백 문자도 아닐 시 오류
            throw Error{ErrorType::SYNTAX, std::wstring(L"어떻게 이게 리슝좍이냐!")};
        }
    }

    // 렉싱이 끝난 후 버퍼에 문자열이 남아있으면 안 됨
    if(!buffer.empty())
    {
        throw Error{ErrorType::SYNTAX, std::wstring(L"어떻게 이게 리슝좍이냐!")};
    }

    if(prevTokenType != TokenType::END_LINE)
    {
        tokenList.push_back(Token{TokenType::END_LINE, 0});
    }

    return tokenList;
}

Lexer::Lexer()
{
    tokenCheckList = {
        TokenCheck{
            TokenType::START,
            createTokenCheckFuncByString({L"교주님"}, 0),
            getZeroValueFunc()
        },
        TokenCheck{
            TokenType::NUMBER,
            createTokenCheckFuncBySeq(L'좍', L"좌아악"),
            getLengthValueFunc()
        },
        TokenCheck{
            TokenType::VAR,
            createTokenCheckFuncBySeq(L'슝', L"슈우웅"),
            getLengthValueFunc()
        },
        TokenCheck{
            TokenType::ADD,
            createTokenCheckFuncByString({L"~"}, 0),
            getZeroValueFunc()
        },
        TokenCheck{
            TokenType::SUB,
            createTokenCheckFuncByString({L";"}, 0),
            getZeroValueFunc()
        },
        TokenCheck{
            TokenType::MUL,
            createTokenCheckFuncByString({L","}, 0),
            getZeroValueFunc()
        },
        TokenCheck{
            TokenType::DIV,
            createTokenCheckFuncByString({L"@"}, 0),
            getZeroValueFunc()
        },
        TokenCheck{
            TokenType::PRINT_NUMBER,
            getPrintTokenCheckFunc(true), // 매개변수 - 숫자 출력인지
            getKekekeValueFunc()
        },
        TokenCheck{
            TokenType::PRINT_ASCII,
            getPrintTokenCheckFunc(false), // 매개변수 - 숫자 출력인지
            getKekekeValueFunc()
        },
        TokenCheck{
            TokenType::INPUT,
            getInputTokenCheckFunc(),
            getKekekeValueFunc()
        },
        TokenCheck{
            TokenType::IF,
            createTokenCheckFuncByString({L"하는재미"}, 0),
            getZeroValueFunc()
        },
        TokenCheck{
            TokenType::GOTO,
            createTokenCheckFuncByString({L"에잇"}, 0),
            getKekekeValueFunc()
        }
    };
}
