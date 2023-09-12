.PHONY: clean cleanall

CC = g++

calc: calc.o main.o parser.o lexer.o

main.o:main.cpp parser.hpp

calc.o:calc.cpp

parser.cpp parser.hpp:parser.ypp
	bison -o parser.cpp -d parser.ypp
	
parser.o:parser.cpp

lexer.cpp:lexer.l parser.hpp
	flex -o lexer.cpp lexer.l
	
lexer.o:lexer.cpp

clean:
	rm -f *.o *.obj

cleanall: clean
	rm -f parser.cpp parser.hpp lexer.cpp lexer.hpp calc calc.exe
