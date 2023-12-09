lexer.h: lang.l
	flex lang.l

lexer.c: lang.l
	flex lang.l

parser.c: lang.y
	bison -o parser.c -d -v lang.y

parser.h: lang.y
	bison -o parser.c -d -v lang.y

lib_new.o: lib_new.c lib_new.h
	g++ -c lib_new.c

lang.o: lang.c lang.h lib_new.h
	g++ -c lang.c

parser.o: parser.c parser.h lexer.h lang.h
	g++ -c parser.c

lexer.o: lexer.c lexer.h parser.h lang.h
	g++ -c lexer.c

interpreter.o: interpreter.cpp lang.h lib_new.h
	g++ -c interpreter.cpp

main.o: main.c lexer.h parser.h lang.h
	g++ -c main.c

main: lang.o parser.o lexer.o interpreter.o lib_new.o main.o
	g++ -g -OO lang.o parser.o lexer.o interpreter.o lib_new.o main.o -o main

main1.o: main1.c lexer.h parser.h lang.h
	g++ -c main1.c

main1: lang.o parser.o lexer.o interpreter.o lib_new.o main1.o
	g++ lang.o parser.o lexer.o interpreter.o lib_new.o main1.o -o main1

all: main
#all: main1

clean:
	rm -f lexer.h lexer.c parser.h parser.c *.o main

%.c: %.y

%.c: %.l

.DEFAULT_GOAL := all
