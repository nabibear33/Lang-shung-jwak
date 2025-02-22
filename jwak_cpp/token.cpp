#include "token.h"

inline bool isCalcToken(Token token)
{
    return token.type == TokenType::ADD ||
            token.type == TokenType::SUB ||
            token.type == TokenType::MUL ||
            token.type == TokenType::DIV;
}
