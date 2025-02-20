#include "ast.h"

ASTNode* creaetASTNode()
{
    ASTNode* node = new ASTNode;
    node->type = ASTType::NOP;
    node->value = 0;
    node->left = nullptr;
    node->right = nullptr;

    return node;
}

void freeASTNode(ASTNode* node)
{
    if(node->left != nullptr)
    {
        freeASTNode(node->left);
    }
    if(node->right != nullptr)
    {
        freeASTNode(node->right);
    }
    delete node;
}

void freeAST(ASTSeq ast)
{
    int astSeqLength = ast.seq.size();
    for(int i = 0; i < astSeqLength; i++)
    {
        freeASTNode(ast.seq[i]);
    }
}

bool isCalcNode(ASTNode* node)
{
    return node->type == ASTType::ADD ||
            node->type == ASTType::SUB ||
            node->type == ASTType::MUL ||
            node->type == ASTType::DIV;
}
