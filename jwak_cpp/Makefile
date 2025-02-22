CC = g++
ALL_OBJS = lexer.o token.o parser.o ast.o compiler.o bytecode.o jwak.o error.o

main: main.o $(ALL_OBJS)
	$(CC) -o jwak.exe main.o $(ALL_OBJS)

test: test.o $(ALL_OBJS)
	$(CC) -o test.exe test.o $(ALL_OBJS)

main.o:
	$(CC) -o main.o -c main.cpp

test.o:
	$(CC) -o test.o -c test.cpp

lexer.o:
	$(CC) -o lexer.o -c lexer.cpp

token.o:
	$(CC) -o token.o -c token.cpp

parser.o:
	$(CC) -o parser.o -c parser.cpp

ast.o:
	$(CC) -o ast.o -c ast.cpp

compiler.o:
	$(CC) -o compiler.o -c compiler.cpp

jwak.o:
	$(CC) -o jwak.o -c jwak.cpp

bytecode.o:
	$(CC) -o bytecode.o -c bytecode.cpp

error.o:
	$(CC) -o error.o -c error.cpp

clean:
	del *.o *.exe
