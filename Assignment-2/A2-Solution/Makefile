all : sclp

sclp : lex.yy.o y.tab.o main.o
	g++ -g -o sclp lex.yy.o y.tab.o main.o -ly -ll

main.o : main.cpp
	g++ -c -g main.cpp

y.tab.o y.tab.h : parser.y
	yacc -dv -Wcounterexamples parser.y
	g++ -c -g y.tab.c

lex.yy.o : scanner.l y.tab.h
	lex scanner.l
	g++ -c -g lex.yy.c

clean :
	rm -f *.o lex.yy.* *.tab.h *.tab.c y.output *.log *.toks exp? a.out sclp
