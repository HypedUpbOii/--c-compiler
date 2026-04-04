all : sclp

gdb : sclp
	gdb sclp

test:
	bash test-tac.sh

sclp : parser.tab.o lex.yy.o lexer.o common_utils.o symbol_table.o output_handler.o argument_handler.o spim.o register.o rtl.o tac.o ast.o main.o
	g++ -fsanitize=address -g -o sclp parser.tab.o lex.yy.o lexer.o common_utils.o symbol_table.o output_handler.o argument_handler.o spim.o register.o rtl.o tac.o ast.o main.o -ly -ll

parser.tab.cc parser.tab.hh location.hh : parser.y
	bison -dv -Wcounterexamples parser.y

parser.tab.o : parser.tab.cc
	g++ -fsanitize=address -c -g parser.tab.cc

lex.yy.o : scanner.l parser.tab.hh lexer.hpp
	flex scanner.l
	g++ -fsanitize=address -c -g lex.yy.cc

lexer.o : lexer.cpp lexer.hpp parser.tab.hh
	g++ -fsanitize=address -c -g lexer.cpp

common_utils.o : common_utils.cpp common_utils.hpp
	g++ -fsanitize=address -c -g common_utils.cpp

symbol_table.o: symbol_table.cpp symbol_table.hpp common_utils.hpp
	g++ -fsanitize=address -c -g symbol_table.cpp

output_handler.o : output_handler.cpp output_handler.hpp common_utils.hpp
	g++ -fsanitize=address -c -g output_handler.cpp

argument_handler.o : argument_handler.cpp argument_handler.hpp common_utils.hpp
	g++ -fsanitize=address -c -g argument_handler.cpp

spim.o : spim.cpp spim.hpp common_utils.hpp
	g++ -fsanitize=address -c -g spim.cpp

register.o: register.cpp register.hpp tac_operand.hpp symbol_table.hpp common_utils.hpp
	g++ -fsanitize=address -c -g register.cpp

rtl.o: rtl.cpp rtl.hpp register.hpp symbol_table.hpp common_utils.hpp spim.hpp
	g++ -fsanitize=address -c -g rtl.cpp

tac.o: tac.cpp tac.hpp rtl.hpp register.hpp symbol_table.hpp common_utils.hpp spim.hpp
	g++ -fsanitize=address -c -g tac.cpp

ast.o: ast.cpp ast.hpp tac.hpp rtl.hpp register.hpp symbol_table.hpp common_utils.hpp spim.hpp
	g++ -fsanitize=address -c -g ast.cpp

main.o : main.cpp argument_handler.cpp lexer.hpp parser.tab.hh output_handler.hpp common_utils.hpp 
	g++ -fsanitize=address -c -g main.cpp

clean :
	rm -f *.o lex.yy.* *.tab.h *.tab.c *.output *.log *.ast *.hh *.toks *.tac *.rtl exp? *.tab.cc a.out sclp

.PHONY: all clean gdb test
