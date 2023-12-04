all: lexer.o
	g++ -std=c++11 main.cpp lexer.o -o main
lexer.o: lexer.cpp
	g++ -std=c++11 -c lexer.cpp -o lexer.o
clean:
	rm -rf *.o main