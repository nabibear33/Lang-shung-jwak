#include <iostream>
#include <string>

#include "lexer.h"
#include "token.h"
#include "parser.h"
#include "ast.h"
#include "compiler.h"
#include "bytecode.h"
#include "jwak.h"
#include "error.h"

int repl()
{
    wprintf(L"LANG-SHUNG-JWAK REPL\nREPL 모드에선 goto문 사용이 불가합니다.\n");

    Lexer lexer;
    Parser parser;
    Compiler compiler;
    Jwak jwak;

    while(1)
    {
        std::wstring code;
        printf(">> ");
        std::wcin >> code;

        try
        {
            TokenList tokenList = lexer.lexing(code);
            tokenList.insert(tokenList.begin(), Token{TokenType::START, 0});

            ASTSeq ast = parser.parsing(tokenList);
            for(ASTNode* node : ast.seq)
            {
                if(node->type == ASTType::GOTO)
                {
                    throw Error{ErrorType::RUNTIME, L"REPL 모드에선 goto문 사용이 불가합니다."};
                }
                else if(node->type == ASTType::IF && node->left != nullptr &&
                        node->left->type == ASTType::GOTO)
                {
                    throw Error{ErrorType::RUNTIME, L"REPL 모드에선 goto문 사용이 불가합니다."};
                }
            }

            Bytecode bytecode = compiler.compile(ast);

            jwak.run(bytecode);

            std::cout << std::endl;

            freeAST(ast);
        }
        catch(Error error)
        {
            printError(error);
        }
    }

    return 0;
}

int executeCodeFromFile(const char* filename)
{
    // 파일 읽기를 c++ 방식으로 구현하면 버그가 여기저기서 터져서 c 방식으로 구현
    wchar_t buffer[513];
    FILE* file = fopen(filename, "r,ccs=UTF-8");

    if(file == NULL)
    {
        wprintf(L"FileNotFoundError: 어떻게 이게 리슝좍이냐!\n");
    }

    std::wstring code = L"";

    size_t byteCount;
    while((byteCount = fread(buffer, sizeof(wchar_t), 512, file)) > 0)
    {
        buffer[byteCount] = L'\0';
        code += buffer;
    }

    fclose(file);

    try {
        Lexer lexer;
        Parser parser;
        Compiler compiler;
        Jwak jwak;

        TokenList tokenList = lexer.lexing(code);

        ASTSeq ast = parser.parsing(tokenList);

        Bytecode bytecode = compiler.compile(ast);

        jwak.run(bytecode);
    }
    catch(Error error)
    {
        printError(error);
        return -1;
    }

    return 0;
}

int main(int argc, char** argv)
{
    setlocale(LC_ALL, "");

    if(argc == 1)
    {
        return repl();
    }
    else if(argc == 2)
    {
        return executeCodeFromFile(argv[1]);
    }

    return 0;
}
