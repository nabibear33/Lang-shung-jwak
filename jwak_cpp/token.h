#ifndef TOKEN_H
#define TOKEN_H

#include <vector>

enum class TokenType
{
    START,
    NUMBER,
    VAR,
    ADD,
    SUB,
    MUL,
    DIV,
    PRINT_ASCII,
    PRINT_NUMBER,
    INPUT,
    IF,
    GOTO, // up, down 여부는 파싱 과정에 한 줄에 goto문이 몇 개 있는가로 구분
    END_LINE,
    NONE // 렉싱 과정에서 사용되는 토큰, 렉싱 결과에는 포함되지 않음
};

typedef struct
{
    TokenType type;
    int value;
}Token;

typedef std::vector<Token> TokenList;

inline bool isCalcToken(Token token);

#endif
