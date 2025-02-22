#ifndef AST_H
#define AST_H

#include <vector>

enum class ASTType
{
    VALUE,
    VAR,
    SET,
    ADD,
    SUB,
    MUL,
    DIV,
    PRINT_ASCII,
    PRINT_NUMBER,
    INPUT,
    IF,
    GOTO,
    NOP // x86 아키텍처의 nop op랑 같은 역할
};

typedef struct ASTNode
{
    ASTType type;
    int value;

    ASTNode* left;
    ASTNode* right;
}ASTNode;

typedef struct
{
    std::vector<ASTNode*> seq;
}ASTSeq;

ASTNode* creaetASTNode();

void freeASTNode(ASTNode* node);
void freeAST(ASTSeq ast);

bool isCalcNode(ASTNode* node);

#endif
