all : sclp

sclp : lex.yy.o parser.tab.o common_utils.o lexer.o main.o symbol_table.o ast.o tac.o rtl.o register.o argument_handler.o output_handler.o
	g++ -fsanitize=address -g -o sclp lex.yy.o common_utils.o parser.tab.o lexer.o main.o symbol_table.o ast.o tac.o rtl.o register.o argument_handler.o output_handler.o -ly -ll

main.o : main.cpp defs.hpp argument_handler.hpp output_handler.hpp ast.hpp lexer.hpp parser.tab.hh location.hh
	g++ -fsanitize=address -c -g main.cpp

parser.tab.o : parser.tab.cc
	g++ -fsanitize=address -c -g parser.tab.cc

parser.tab.cc parser.tab.hh location.hh : parser.y
	bison -dv -Wcounterexamples parser.y
	mv parser.output y.output

gdb : sclp
	gdb sclp

test:
	bash test-tac.sh

lex.yy.o : scanner.l parser.tab.hh lexer.hpp
	flex scanner.l
	g++ -fsanitize=address -c -g lex.yy.cc

lexer.o : lexer.cpp lexer.hpp parser.tab.hh
	g++ -fsanitize=address -c -g lexer.cpp

symbol_table.o: symbol_table.hpp common_utils.hpp symbol_table.cpp
	g++ -fsanitize=address -c -g symbol_table.cpp

ast.o: ast.hpp common_utils.hpp symbol_table.hpp tac.hpp ast.cpp
	g++ -fsanitize=address -c -g ast.cpp

tac.o: tac.hpp common_utils.hpp symbol_table.hpp tac.cpp
	g++ -fsanitize=address -c -g tac.cpp

rtl.o: common_utils.hpp symbol_table.hpp register.hpp rtl.hpp rtl.cpp
	g++ -fsanitize=address -c -g rtl.cpp
	
register.o: common_utils.hpp symbol_table.hpp tac.hpp register.hpp register.cpp
	g++ -fsanitize=address -c -g register.cpp

common_utils.o : common_utils.cpp common_utils.hpp
	g++ -fsanitize=address -c -g common_utils.cpp

argument_handler.o : argument_handler.cpp argument_handler.hpp common_utils.hpp
	g++ -fsanitize=address -c -g argument_handler.cpp

output_handler.o : output_handler.cpp output_handler.hpp common_utils.hpp
	g++ -fsanitize=address -c -g output_handler.cpp

clean :
	rm -f *.o lex.yy.* *.tab.h *.tab.c *.output *.log *.ast *.hh *.toks *.tac exp? *.tab.cc a.out sclp

.PHONY: all clean gdb test
