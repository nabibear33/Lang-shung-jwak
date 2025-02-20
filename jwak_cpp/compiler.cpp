#include "compiler.h"

#include <iostream>
#include "error.h"

inline void Compiler::appendBytecode(Bytecode* code1, Bytecode* code2)
{
    Bytecode::iterator it = code1->end();
    code1->insert(it, code2->begin(), code2->end());
}

inline OpCode Compiler::nodeTypeToInst(ASTType type)
{
    switch (type)
    {
        case ASTType::VALUE:
        {
            return OpCode::PUSH_VALUE;
        }
        case ASTType::VAR:
        {
            return OpCode::PUSH_VAR;
        }

        case ASTType::SET:
        {
            return OpCode::SET;
        }

        case ASTType::ADD:
        {
            return OpCode::ADD;
        }
        case ASTType::SUB:
        {
            return OpCode::SUB;
        }
        case ASTType::MUL:
        {
            return OpCode::MUL;
        }
        case ASTType::DIV:
        {
            return OpCode::DIV;
        }

        case ASTType::PRINT_ASCII:
        {
            return OpCode::PRINT_ASCII;
        }
        case ASTType::PRINT_NUMBER:
        {
            return OpCode::PRINT_NUMBER;
        }

        case ASTType::INPUT:
        {
            return OpCode::INPUT;
        }

        case ASTType::IF:
        {
            return OpCode::IF;
        }

        case ASTType::GOTO:
        {
            return OpCode::GOTO;
        }

        // case ASTType::NOP:
        // {
        //     return OpCode::NOP;
        // }
    }
}

inline void Compiler::nodeToBytecode(ASTNode* node, Bytecode* bytecode)
{
    switch(node->type)
    {
        case ASTType::VALUE:
        {
            bytecode->push_back((unsigned char)(this->nodeTypeToInst(node->type)));

            Byte4 byte4;
            byte4.i = node->value;
            bytecode->push_back(byte4.c[0]);
            bytecode->push_back(byte4.c[1]);
            bytecode->push_back(byte4.c[2]);
            bytecode->push_back(byte4.c[3]);

            this->currentByteIndex += 5;

            break;
        }

        case ASTType::SET:
        case ASTType::PRINT_ASCII:
        case ASTType::PRINT_NUMBER:
        case ASTType::INPUT:
        case ASTType::VAR:
        {
            if(node->value == -1)
            {
                throw Error{ErrorType::SYNTAX, std::wstring(L"어떻게 이게 리슝좍이냐!")};
            }

            bytecode->push_back((unsigned char)this->nodeTypeToInst(node->type));
            bytecode->push_back((unsigned char)node->value);

            this->currentByteIndex += 2;

            break;
        }

        case ASTType::ADD:
        case ASTType::SUB:
        case ASTType::MUL:
        case ASTType::DIV:
        case ASTType::IF:
        {
            bytecode->push_back((unsigned char)this->nodeTypeToInst(node->type));

            this->currentByteIndex += 1;

            break;
        }

        case ASTType::GOTO:
        {
            bytecode->push_back((unsigned char)(this->nodeTypeToInst(node->type)));

            bytecode->push_back(0);
            bytecode->push_back(0);
            bytecode->push_back(0);
            bytecode->push_back(0);

            this->gotoDataList.push_back(GotoData{this->currentByteIndex, node->value, this->currentLine});
            this->currentByteIndex += 5;

            break;
        }

        case ASTType::NOP:
        {
            // 내일 NOP과 빈 줄이 goto에게 어떤 영향을 주는지 생각
            break;
        }
    }
}

Bytecode Compiler::nodeCompile(ASTNode* node)
{
    Bytecode bytecode;

    if(node->type == ASTType::IF)
    {
        if(node->right != nullptr)
        {
            Bytecode nodeBytecode = this->nodeCompile(node->right);
            this->appendBytecode(&bytecode, &nodeBytecode);
        }
        else
        {
            ASTNode* newNode = creaetASTNode();
            newNode->type = ASTType::VALUE;
            newNode->value = 0;

            Bytecode nodeBytecode = this->nodeCompile(newNode);
            this->appendBytecode(&bytecode, &nodeBytecode);

            freeASTNode(newNode);
        }

        this->nodeToBytecode(node, &bytecode);

        if(node->left != nullptr)
        {
            Bytecode nodeBytecode = this->nodeCompile(node->left);
            this->appendBytecode(&bytecode, &nodeBytecode);
        }
    }
    else
    {
        if(node->left != nullptr)
        {
            Bytecode nodeBytecode = this->nodeCompile(node->left);
            this->appendBytecode(&bytecode, &nodeBytecode);
        }

        if(node->right != nullptr)
        {
            Bytecode nodeBytecode = this->nodeCompile(node->right);
            this->appendBytecode(&bytecode, &nodeBytecode);
        }
        else
        {
            if(node->type == ASTType::SET)
            {
                ASTNode* newNode = creaetASTNode();
                newNode->type = ASTType::VALUE;
                newNode->value = 0;

                Bytecode nodeBytecode = this->nodeCompile(newNode);
                this->appendBytecode(&bytecode, &nodeBytecode);

                freeASTNode(newNode);
            }
        }

        this->nodeToBytecode(node, &bytecode);
    }

    return bytecode;
}

Bytecode Compiler::compile(ASTSeq ast)
{
    this->currentByteIndex = 0;
    this->currentLine = 0;
    this->gotoDataList.clear();
    this->linePos.clear();

    Bytecode bytecode;

    for(ASTNode* node : ast.seq)
    {
        try
        {
            this->linePos.push_back(this->currentByteIndex);

            // printf("line: %-2d | bytePos: %-3d | lineListSize: %-2d\n", this->currentLine, this->currentByteIndex, this->linePos.size());

            Bytecode nodeBytecode = this->nodeCompile(node);
            this->appendBytecode(&bytecode, &nodeBytecode);

            this->currentLine++;
        }
        catch(Error error)
        {
            throw error;
        }
    }
    this->linePos.push_back(this->currentByteIndex);

    for(GotoData data : this->gotoDataList)
    {
        Byte4 byte;
        int targetLine = std::max(std::min(data.line + data.value, this->currentLine), 0);
        int dist = this->linePos[targetLine];// - this->linePos[data.line];
        // printf("currentLine: %-2d | targetLine: %-2d | dist = %3d - %3d = %4d | targetOpCode %-2d\n", data.line, targetLine, this->linePos[targetLine], this->linePos[data.line], dist, bytecode[data.value + dist + 1]);

        byte.i = dist;

        int i = data.point + 1;
        bytecode[i++] = byte.c[0];
        bytecode[i++] = byte.c[1];
        bytecode[i++] = byte.c[2];
        bytecode[i++] = byte.c[3];
    }

    return bytecode;
}
