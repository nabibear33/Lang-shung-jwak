#ifndef JWAK_H
#define JWAK_H

#define MAX_VARIABLE_SIZE 255

#include <string>
#include <vector>
#include <stack>
#include <queue>
#include "error.h"
#include "lexer.h"
#include "bytecode.h"

class Jwak
{
private:
    Lexer lexer;
    std::vector<int> memory;
    std::stack<int> stack;
    std::queue<int> inputQueue;

    inline void addToInputQueue(std::wstring value);

public:
    Jwak();

    void appendInputQueue(std::wstring str);
    int getInputQueue();
    int getInputQueueSize();

    void push(int num);
    int pop();

    void run(Bytecode bytecode);
    void reset();
};

#endif
