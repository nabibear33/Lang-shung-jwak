#include "jwak.h"

#include <algorithm>

Jwak::Jwak()
{
    this->memory.resize(MAX_VARIABLE_SIZE);
}

void Jwak::push(int value)
{
    this->stack.push(value);
}

int Jwak::pop()
{
    int value = this->stack.top();
    this->stack.pop();
    return value;
}

inline void Jwak::addToInputQueue(std::wstring value)
{
    bool isNumber = true;
    int number = 0;
    for(wchar_t c : value)
    {
        if(c < L'0' || c > L'9')
        {
            isNumber = false;
            break;
        }
        number *= 10;
        number += (int)c - L'0';
    }

    if(isNumber)
    {
        this->inputQueue.push(number);
    }
    else
    {
        for(wchar_t c : value)
        {
            this->inputQueue.push((int)c);
        }
    }
}

void Jwak::appendInputQueue(std::wstring str)
{
    std::wstring buffer;
    for(wchar_t c : str)
    {
        if(c == L' ' || c == L'\n')
        {
            this->addToInputQueue(buffer);
            buffer = L"";
            continue;
        }

        buffer += c;
    }

    if(!buffer.empty())
    {
        this->addToInputQueue(buffer);
    }
}

int Jwak::getInputQueue()
{
    int value = this->inputQueue.front();
    this->inputQueue.pop();
    return value;
}

int Jwak::getInputQueueSize()
{
    return this->inputQueue.size();
}


void Jwak::run(Bytecode bytecode)
{
    int i = 0;
    int length = bytecode.size();
    bool passNextCode = false;

    while(i < length)
    {
        OpCode op = (OpCode)bytecode[i];
        int size = instSize[bytecode[i]];

        if(passNextCode)
        {
            i += size;
            passNextCode = false;
            continue;
        }

        switch(op)
        {
            case OpCode::SET:
            {
                int index = bytecode[i + 1];
                this->memory[index] = this->pop();

                break;
            }

            case OpCode::ADD:
            {
                int value2 = this->pop();
                int value1 = this->pop();
                this->push(value1 + value2);

                break;
            }

            case OpCode::SUB:
            {
                int value2 = this->pop();
                int value1 = this->pop();
                this->push(value1 - value2);

                break;
            }

            case OpCode::MUL:
            {
                int value2 = this->pop();
                int value1 = this->pop();
                this->push(value1 * value2);

                break;
            }

            case OpCode::DIV:
            {
                int value2 = this->pop();
                if(value2 == 0)
                {
                    throw Error{ErrorType::ZERO_DIVISION, std::wstring(L"어떻게 이게 리슝좍이냐!")};
                }

                int value1 = this->pop();
                this->push(value1 / value2);

                break;
            }

            case OpCode::PRINT_ASCII:
            {
                int index = bytecode[i + 1];
                int value = this->memory[index];
                putwchar(value);

                break;
            }

            case OpCode::PRINT_NUMBER:
            {
                int index = bytecode[i + 1];
                int value = this->memory[index];
                printf("%d", value);

                break;
            }

            case OpCode::INPUT:
            {
                if(this->getInputQueueSize() == 0)
                {
                    std::wstring input;
                    std::wcin >> input;
                    this->appendInputQueue(input);
                }

                int index = bytecode[i + 1];
                this->memory[index] = this->getInputQueue();

                break;
            }

            case OpCode::IF:
            {
                int value = this->pop();
                if(value != 0)
                {
                    passNextCode = true;
                }

                break;
            }

            case OpCode::GOTO:
            {
                Byte4 byte;
                byte.c[0] = bytecode[i + 1];
                byte.c[1] = bytecode[i + 2];
                byte.c[2] = bytecode[i + 3];
                byte.c[3] = bytecode[i + 4];

                // size = byte.i;
                i = byte.i;
                size = 0;

                break;
            }

            case OpCode::PUSH_VALUE:
            {
                Byte4 byte;
                byte.c[0] = bytecode[i + 1];
                byte.c[1] = bytecode[i + 2];
                byte.c[2] = bytecode[i + 3];
                byte.c[3] = bytecode[i + 4];

                this->push(byte.i);

                break;
            }

            // case OpCode::NOP:
            // {
            //     break;
            // }

            case OpCode::PUSH_VAR:
            {
                int index = bytecode[i + 1];
                int value = this->memory[index];
                this->push(value);

                break;
            }

            default:
            {
                throw Error{ErrorType::RUNTIME, std::wstring(L"인터프리터가 바이트 코드를 읽던 중 알 수 없는 명령어를 발견하였습니다. 이는 코드의 문제가 아닌 인터프리터 자체의 문제이기 때문에 코드와 함께 이슈를 날려주시기 바랍니다.")};
            }
        }

        i += size;
    }
}

void Jwak::reset()
{
    while(!this->stack.empty()) {
        this->stack.pop();
    }
    std::fill(this->memory.begin(), this->memory.end(), 0);
}
