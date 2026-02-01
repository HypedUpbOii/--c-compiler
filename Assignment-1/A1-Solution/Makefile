all : sclp

sclp : lex.yy.o y.tab.o main.o
	g++ -g -o sclp lex.yy.o y.tab.o main.o -ly -ll

main.o : main.cpp
	g++ -c -g main.cpp

y.tab.o y.tab.h : parser.y
	yacc -dv -Wcounterexamples parser.y
	gcc -c -g y.tab.c

lex.yy.o : scanner.l y.tab.h
	lex scanner.l
	gcc -c -g lex.yy.c

scan : scanner.l
	lex scanner.l
	gcc -g lex.yy.c -ll -o scan

clean :
	rm -f *.o lex.yy.* *.tab.h *.tab.c y.output exp? a.out scan sclp

