#include "parser.h"

#include "error.h"

inline ASTType Parser::tokenTypeToNodeType(TokenType type)
{
    switch(type)
    {
        // calc
        case TokenType::ADD:
        {
            return ASTType::ADD;
        }
        case TokenType::SUB:
        {
            return ASTType::SUB;
        }
        case TokenType::MUL:
        {
            return ASTType::MUL;
        }
        case TokenType::DIV:
        {
            return ASTType::DIV;
        }

        // print
        case TokenType::PRINT_ASCII:
        {
            return ASTType::PRINT_ASCII;
        }
        case TokenType::PRINT_NUMBER:
        {
            return ASTType::PRINT_NUMBER;
        }
    }
}

ASTSeq Parser::parsing(TokenList tokenList)
{
    ASTSeq ast;

    if(tokenList.empty())
    {
        throw Error{ErrorType::SYNTAX, std::wstring(L"어떻게 이게 리슝좍이냐!")};
    }
    if(tokenList[0].type != TokenType::START)
    {
        throw Error{ErrorType::SYNTAX, std::wstring(L"어떻게 이게 리슝좍이냐!")};
    }

    int tokenLength = tokenList.size();
    ASTNode* currentNode = nullptr;
    ASTNode* parentNode = nullptr;
    ASTNode* rootNode = nullptr;

    bool noMoreNode = false;

    for(int i = 0; i < tokenLength; i++)
    {
        Token token = tokenList[i];

        switch(token.type)
        {
            case TokenType::VAR:
            {
                if(noMoreNode)
                {
                    freeAST(ast);
                    throw Error{ErrorType::SYNTAX, std::wstring(L"어떻게 이게 리슝좍이냐!")};
                }

                if(rootNode == nullptr)
                {
                    // SET
                    ASTNode* node = creaetASTNode();
                    node->type = ASTType::SET;
                    node->value = token.value - 1; // 변수 인덱스는 0부터 시작이지만 렉싱 과정에선 1부터 셈

                    currentNode = node;
                    parentNode = node;
                    rootNode = node;
                }
                else
                {
                    if(currentNode != nullptr && currentNode->right != nullptr)
                    {
                        freeAST(ast);
                        throw Error{ErrorType::SYNTAX, std::wstring(L"어떻게 이게 리슝좍이냐!")};
                    }

                    ASTNode* node = creaetASTNode();
                    node->type = ASTType::VAR;
                    node->value = token.value - 1; // 변수 인덱스는 0부터 시작이지만 렉싱 과정에선 1부터 셈

                    currentNode->right = node;

                    currentNode = node;
                }

                break;
            }

            case TokenType::NUMBER:
            {
                if(noMoreNode)
                {
                    freeAST(ast);
                    throw Error{ErrorType::SYNTAX, std::wstring(L"어떻게 이게 리슝좍이냐!")};
                }

                if(rootNode == nullptr)
                {
                    freeAST(ast);
                    throw Error{ErrorType::SYNTAX, std::wstring(L"어떻게 이게 리슝좍이냐!")};
                }
                if(currentNode != nullptr && currentNode->right != nullptr)
                {
                    freeAST(ast);
                    throw Error{ErrorType::SYNTAX, std::wstring(L"어떻게 이게 리슝좍이냐!")};
                }

                ASTNode* node = creaetASTNode();
                node->type = ASTType::VALUE;
                node->value = token.value;

                currentNode->right = node;

                currentNode = node;
                break;
            }

            case TokenType::ADD:
            case TokenType::SUB:
            case TokenType::MUL:
            case TokenType::DIV:
            {
                if(noMoreNode)
                {
                    freeAST(ast);
                    throw Error{ErrorType::SYNTAX, std::wstring(L"어떻게 이게 리슝좍이냐!")};
                }

                if(currentNode == nullptr)
                {
                    freeAST(ast);
                    throw Error{ErrorType::SYNTAX, std::wstring(L"어떻게 이게 리슝좍이냐!")};
                }

                if(currentNode->type != ASTType::VALUE && currentNode->type != ASTType::VAR)
                {
                    freeAST(ast);
                    throw Error{ErrorType::SYNTAX, std::wstring(L"어떻게 이게 리슝좍이냐!")};
                }

                ASTNode* node = creaetASTNode();
                node->type = this->tokenTypeToNodeType(token.type);
                if(!isCalcNode(parentNode->right))
                {
                    node->left = currentNode;
                }
                else
                {
                    node->left = parentNode->right;
                }
                currentNode = node;
                parentNode->right = node;

                break;
            }

            case TokenType::PRINT_ASCII:
            case TokenType::PRINT_NUMBER:
            {
                if(noMoreNode)
                {
                    freeAST(ast);
                    throw Error{ErrorType::SYNTAX, std::wstring(L"어떻게 이게 리슝좍이냐!")};
                }

                noMoreNode = true;

                ASTNode* node = creaetASTNode();
                node->type = tokenTypeToNodeType(token.type);
                node->value = token.value - 1; // 변수 인덱스는 0부터 시작이지만 렉싱 과정에선 1부터 셈

                rootNode = node;

                break;
            }

            case TokenType::INPUT:
            {
                if(noMoreNode)
                {
                    freeAST(ast);
                    throw Error{ErrorType::SYNTAX, std::wstring(L"어떻게 이게 리슝좍이냐!")};
                }

                noMoreNode = true;

                ASTNode* node = creaetASTNode();
                node->type = ASTType::INPUT;
                node->value = token.value - 1; // 변수 인덱스는 0부터 시작이지만 렉싱 과정에선 1부터 셈

                rootNode = node;

                break;
            }

            case TokenType::GOTO:
            {
                if(tokenList[i - 1].type == TokenType::GOTO)
                {
                    currentNode->value = token.value - currentNode->value;

                    break;
                }

                if(noMoreNode)
                {
                    freeAST(ast);
                    throw Error{ErrorType::SYNTAX, std::wstring(L"어떻게 이게 리슝좍이냐!")};
                }

                noMoreNode = true;

                ASTNode* node = creaetASTNode();
                node->type = ASTType::GOTO;
                node->value = -token.value;

                rootNode = node;
                currentNode = node;

                break;
            }

            case TokenType::IF:
            {
                if(rootNode != nullptr && rootNode->type == ASTType::IF)
                {
                    freeAST(ast);
                    throw Error{ErrorType::SYNTAX, std::wstring(L"어떻게 이게 리슝좍이냐!")};
                }

                ASTNode* node = creaetASTNode();
                node->type = ASTType::IF; 
                node->left = rootNode;

                currentNode = node;
                parentNode = node;
                rootNode = node;

                noMoreNode = false;

                break;
            }

            case TokenType::END_LINE:
            {
                if(rootNode == nullptr)
                {
                    ast.seq.push_back(new ASTNode{ASTType::NOP, 0, nullptr, nullptr});
                }
                else
                {
                    ast.seq.push_back(rootNode);
                    currentNode = nullptr;

                    currentNode = nullptr;
                    parentNode = nullptr;
                    rootNode = nullptr;
                }

                noMoreNode = false;

                break;
            }
        }
    }

    return ast;
}
