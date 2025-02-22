#ifndef COMPILER_H
#define COMPILER_H

#include "bytecode.h"
#include "ast.h"

typedef struct
{
    int point;
    int value;
    int line;
}GotoData;

class Compiler
{
private:
    inline void appendBytecode(Bytecode* code1, Bytecode* code2);
    inline OpCode nodeTypeToInst(ASTType type);

    inline void nodeToBytecode(ASTNode* node, Bytecode* bytecode);

    int currentByteIndex;
    int currentLine;
    int instByteSize;

    std::vector<GotoData> gotoDataList;
    std::vector<int> linePos;

    Bytecode nodeCompile(ASTNode* node);

public:
    Bytecode compile(ASTSeq ast);
};

#endif
