#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "ast.h"

class Parser
{
private:
    inline ASTType tokenTypeToNodeType(TokenType type);

public:
    ASTSeq parsing(TokenList tokenList);
};

#endif
