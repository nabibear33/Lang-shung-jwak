#include <iostream>
#include "lexer.h"
#include "token.h"
#include "parser.h"
#include "ast.h"
#include "compiler.h"
#include "bytecode.h"
#include "jwak.h"
#include "error.h"

void printTokenList(TokenList tokenList)
{
    int tokenLength = tokenList.size();
    for(int i = 0; i < tokenLength; i++)
    {
        Token token = tokenList[i];
        switch(token.type)
        {
            case TokenType::START:
            {
                std::cout << "START | ";
                break;
            }
            case TokenType::NUMBER:
            {
                std::cout << "NUMBER | ";
                break;
            }
            case TokenType::ADD:
            {
                std::cout << "ADD | ";
                break;
            }
            case TokenType::SUB:
            {
                std::cout << "SUB | ";
                break;
            }
            case TokenType::MUL:
            {
                std::cout << "MUL | ";
                break;
            }
            case TokenType::DIV:
            {
                std::cout << "DIV | ";
                break;
            }
            case TokenType::VAR:
            {
                std::cout << "VAR | ";
                break;
            }
            case TokenType::PRINT_ASCII:
            {
                std::cout << "PRINT_ASCII | ";
                break;
            }
            case TokenType::PRINT_NUMBER:
            {
                std::cout << "PRINT_NUMBER | ";
                break;
            }
            case TokenType::INPUT:
            {
                std::cout << "INPUT | ";
                break;
            }
            case TokenType::IF:
            {
                std::cout << "IF | ";
                break;
            }
            case TokenType::GOTO:
            {
                std::cout << "GOTO | ";
                break;
            }
            case TokenType::END_LINE:
            {
                std::cout << "END_LINE | ";
                break;
            }
        }
        std::cout << token.value << std::endl;
    }
}

void printStringWithDepth(std::string str, int depth)
{
    for(int i = 0; i < depth; i++)
    {
        std::cout << "| ";
    }
    std::cout << str << std::endl;
}

std::string astTypeToString(ASTType type)
{
    switch(type)
    {
        case ASTType::VALUE:
            return "VALUE";
        case ASTType::VAR:
            return "VAR";
        case ASTType::SET:
            return "SET";
        case ASTType::ADD:
            return "ADD";
        case ASTType::SUB:
            return "SUB";
        case ASTType::MUL:
            return "MUL";
        case ASTType::DIV:
            return "DIV";
        case ASTType::PRINT_ASCII:
            return "PRINT_ASCII";
        case ASTType::PRINT_NUMBER:
            return "PRINT_NUMBER";
        case ASTType::INPUT:
            return "INPUT";
        case ASTType::IF:
            return "IF";
        case ASTType::GOTO:
            return "GOTO";
        case ASTType::NOP:
            return "NOP";
    }
}

void printASTNode(ASTNode* node, int depth)
{ 
    char buf[512];

    printStringWithDepth("ASTNode", depth);

    sprintf(buf, "type: %s", astTypeToString(node->type).c_str());
    printStringWithDepth(buf, depth + 1);

    sprintf(buf, "value: %d", node->value);
    printStringWithDepth(buf, depth + 1);

    if(node->left != nullptr)
    {
        printStringWithDepth("left:", depth + 1);
        printASTNode(node->left, depth + 2);
    }

    if(node->right != nullptr)
    {
        printStringWithDepth("right:", depth + 1);
        printASTNode(node->right, depth + 2);
    }
}

void printASTSeq(ASTSeq ast)
{
    std::cout << "ASTSeq" << std::endl;
    int edgeLength = ast.seq.size();
    for(int i = 0; i < edgeLength; i++)
    {
        printASTNode(ast.seq[i], 1);
    }
}

void printBytecode(Bytecode bytecode)
{
    for(Byte byte : bytecode)
    {
        std::cout << (int)byte << std::endl;
    }
}

void printBytecode2(Bytecode bytecode)
{
    int i = 0;
    int length = bytecode.size();

    while(i < length)
    {
        int size = instSize[bytecode[i]];
        printf("[%d] %d ", size, int(bytecode[i]));
        i++;
        if(size == 2)
        {
            std::cout << (int)bytecode[i++] << std::endl;
        }
        else if(size == 5)
        {
            Byte4 byte;
            byte.c[0] = bytecode[i++];
            byte.c[1] = bytecode[i++];
            byte.c[2] = bytecode[i++];
            byte.c[3] = bytecode[i++];
            std::cout << byte.i << std::endl;
        }
        else
        {
            std::cout << std::endl;
        }
    }
}

int main()
{
    setlocale(LC_ALL, "");

    std::cout << "test start\n---" << std::endl;

    Lexer lexer;
    Parser parser;
    Compiler compiler;
    Jwak jwak;

    // ----------------------------------------------------------

    // jwak.appendInputQueue(L"1 2");
    // jwak.appendInputQueue(L"3 a\nb");
    // for(int i = 0; i < 5; i++)
    // {
    //     std::cout << jwak.getInputQueue() << std::endl;
    // }

    // return 0;

    // ASTSeq ast;

    // ASTNode* node2 = new ASTNode;
    // node2->type = ASTType::PRINT_NUMBER;
    // node2->value = 100;

    // ASTNode* node3 = new ASTNode;
    // node3->type = ASTType::VAR;
    // node3->value = 0;

    // ASTNode* node1 = new ASTNode;
    // node1->type = ASTType::IF;
    // node1->value = 0;
    // node1->left = node2;
    // node1->right = node3;

    // ast.seq.push_back(node1);
    // printASTSeq(ast, 0);

    // freeAST(ast);

// goto문 버그 해결 될때까지 헛소리 한 줄 추가함
// ---
// 안녕 goto문아?
// 자신의 게임에 "말장난은 정말짱난다"는 말장난을 넣는
// 어느 쯔꾸르 게임 제작자가 있었어
// 넌 그냥 짱나 ㅅㅂ아
// 착하게짤테니한번만봐다오
// deepseek-r1이디버깅을잘한다길래해봤느데아니무슨구구단인데1만나와나참어이가없어서
// deepseek-r1이 코드 이해를 못 했던 거였어
// 딥씩이가 바쁘다고 하네요. AI한테 이렇게 디버깅을 통으로 맡겨본건 처음입니다. 하는 수 없이 o3-mini를 꺼냈습니다. ... 버그네요. 햄버그가 좋은 리스티를 말아서 한대 펴야겠네요. 쓰으으으으읍.... 아 귀여워 으흐흐흫ㅎ흫흫...
// 느아니 왜 안 드러 가는 거야! 하 젠장 되는 일이 하나도 없허. 왜! 나 아까쳤어! 가 머 때문에 이러능 아무도 날 이해 모테! 난 한 번만으라도 행보카고 시픙데 왜 나 @$%*(&는 햄보칼 수가 업서! 이 말도 안 되는 코드는 누가 만든 거야. 너 이러케 goto해야 되는 거야 이마. 이러케, 이러케. 제발 이거 좀 푸러줘헣 나 미찌겠네! 개 같은 경우. 왜, 왜!, 왜!
// 결국 해결 못 하고 goto 위치를 상댓값 말고 절댓값으로 변경

    try
    {
        TokenList tokenList = lexer.lexing(
L"교주님\n"
L"\n"
L"슝\n"
L"슈웅좍\n"
L"\n"
L"순수따잇ㅋㅋㅋ\n"
L"\n"
L"슈우우웅\n"
L"슈우우우웅\n"
L"슈우우우우웅좌아아아아아아아아악\n"
L"\n"
L"비비보호막따잇ㅋ\n"
L"비비따잇ㅋㅋㅋㅋㅋㅋ\n"
L"슈우우우웅슝~슈웅\n"
L"\n"
L"에잇에잇ㅋㅋㅋㅋㅋㅋㅋ하는재미슈우웅;슈우우웅;좍\n"
L"\n"
L"슝슈웅\n"
L"슈웅슈우우우웅\n"
L"슈우우웅슈우우웅~좍\n"
L"\n"
L"에잇ㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋ"
        );

        printTokenList(tokenList);

        ASTSeq ast = parser.parsing(tokenList);

        printASTSeq(ast);

        Bytecode bytecode = compiler.compile(ast);

        printBytecode2(bytecode);

        jwak.run(bytecode);

        freeAST(ast);
    }
    catch(Error error)
    {
        printError(error);
    }

    std::cout << "\n---\ntext end" << std::endl;

    return 0;
}
