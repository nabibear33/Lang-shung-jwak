#ifndef BYTECODE_H
#define BYTECODE_H

#include <vector>

enum class OpCode
{
    SET = 0,
    ADD = 1,
    SUB = 2,
    MUL = 3,
    DIV = 4,
    PRINT_ASCII = 5,
    PRINT_NUMBER = 6,
    INPUT = 7,
    IF = 8,
    GOTO = 9,
    PUSH_VALUE = 10,
    NOP = 11,
    PUSH_VAR = 12
};

extern int instSize[13];

typedef unsigned char Byte;
typedef std::vector<Byte> Bytecode;

typedef union
{
    unsigned char c[4];
    int i;
}Byte4;

#endif
