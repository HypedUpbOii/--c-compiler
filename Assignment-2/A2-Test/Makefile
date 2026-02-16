all : sclp

sclp : clean lex.yy.o parser.tab.o lexer.o main.o argument_handler.o output_handler.o
	g++ -g -o sclp lex.yy.o parser.tab.o lexer.o main.o argument_handler.o output_handler.o -ly -ll

main.o : main.cpp
	g++ -c -g main.cpp

parser.tab.o : parser.tab.cc
	g++ -c -g parser.tab.cc

parser.tab.cc parser.tab.hh location.hh : parser.y
	bison -dv -Wcounterexamples parser.y

lex.yy.o : scanner.l parser.tab.hh
	flex scanner.l
	g++ -c -g lex.yy.cc

lexer.o : lexer.cpp
	g++ -c -g lexer.cpp

argument_handler.o : argument_handler.cpp
	g++ -c -g argument_handler.cpp

output_handler.o : output_handler.cpp
	g++ -c -g output_handler.cpp

clean :
	rm -f *.o lex.yy.* *.tab.h *.tab.c *.output *.log *.ast *.hh *.toks exp? *.tab.cc a.out sclp
