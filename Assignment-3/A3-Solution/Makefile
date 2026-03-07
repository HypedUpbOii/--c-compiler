all : sclp

sclp : lex.yy.o parser.tab.o common_utils.o lexer.o main.o symbol_table.o ast.o tac.o argument_handler.o output_handler.o
	g++ -g -o sclp lex.yy.o common_utils.o parser.tab.o lexer.o main.o symbol_table.o ast.o tac.o argument_handler.o output_handler.o -ly -ll

main.o : main.cpp defs.hpp argument_handler.hpp output_handler.hpp ast.hpp lexer.hpp parser.tab.hh location.hh
	g++ -c -g main.cpp

parser.tab.o : parser.tab.cc
	g++ -c -g parser.tab.cc

parser.tab.cc parser.tab.hh location.hh : parser.y
	bison -dv -Wcounterexamples parser.y
	mv parser.output y.output

lex.yy.o : scanner.l parser.tab.hh lexer.hpp
	flex scanner.l
	g++ -c -g lex.yy.cc

lexer.o : lexer.cpp lexer.hpp parser.tab.hh
	g++ -c -g lexer.cpp

symbol_table.o: symbol_table.hpp common_utils.hpp symbol_table.cpp
	g++ -c -g symbol_table.cpp

ast.o: ast.hpp common_utils.hpp symbol_table.hpp tac.hpp ast.cpp
	g++ -c -g ast.cpp

tac.o: tac.hpp common_utils.hpp symbol_table.hpp tac.cpp
	g++ -c -g tac.cpp

common_utils.o : common_utils.cpp common_utils.hpp
	g++ -c -g common_utils.cpp

argument_handler.o : argument_handler.cpp argument_handler.hpp common_utils.hpp
	g++ -c -g argument_handler.cpp

output_handler.o : output_handler.cpp output_handler.hpp common_utils.hpp
	g++ -c -g output_handler.cpp

clean :
	rm -f *.o lex.yy.* *.tab.h *.tab.c *.output *.log *.ast *.hh *.toks exp? *.tab.cc a.out sclp
