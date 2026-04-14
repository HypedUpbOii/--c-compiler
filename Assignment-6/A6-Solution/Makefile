all : sclp

gdb : sclp
	gdb sclp

test:
	bash test-tac.sh

sclp : parser.tab.o lex.yy.o lexer.o common_utils.o symbol_table.o output_handler.o argument_handler.o spim.o register.o rtl.o tac.o ast.o main.o
	g++ -fsanitize=address -g -o sclp parser.tab.o lex.yy.o lexer.o common_utils.o symbol_table.o output_handler.o argument_handler.o spim.o register.o rtl.o tac.o ast.o main.o -ly -ll

argument_handler.hpp : common_utils.hpp

output_handler.hpp : common_utils.hpp

symbol_table.hpp : common_utils.hpp

tac_operand.hpp : symbol_table.hpp

register.hpp : tac_operand.hpp

spim.hpp : register.hpp

rtl.hpp : spim.hpp

tac.hpp : rtl.hpp

ast.hpp : tac.hpp

lexer.hpp : ast.hpp

parser.tab.cc parser.tab.hh location.hh : parser.y
	bison -dv -Wcounterexamples parser.y

parser.tab.o : parser.tab.cc ast.hpp
	g++ -fsanitize=address -g -c parser.tab.cc

lex.yy.o : scanner.l parser.tab.hh lexer.hpp
	flex scanner.l
	g++ -fsanitize=address -g -c lex.yy.cc

lexer.o : lexer.cpp lexer.hpp parser.tab.hh
	g++ -fsanitize=address -g -c lexer.cpp

common_utils.o : common_utils.cpp common_utils.hpp
	g++ -fsanitize=address -g -c common_utils.cpp

symbol_table.o: symbol_table.cpp symbol_table.hpp
	g++ -fsanitize=address -g -c symbol_table.cpp

output_handler.o : output_handler.cpp output_handler.hpp
	g++ -fsanitize=address -g -c output_handler.cpp

argument_handler.o : argument_handler.cpp argument_handler.hpp
	g++ -fsanitize=address -g -c argument_handler.cpp

spim.o : spim.cpp spim.hpp
	g++ -fsanitize=address -g -c spim.cpp

register.o: register.cpp register.hpp
	g++ -fsanitize=address -g -c register.cpp

rtl.o: rtl.cpp rtl.hpp
	g++ -fsanitize=address -g -c rtl.cpp

tac.o: tac.cpp tac.hpp
	g++ -fsanitize=address -g -c tac.cpp

ast.o: ast.cpp ast.hpp
	g++ -fsanitize=address -g -c ast.cpp

main.o : main.cpp argument_handler.cpp lexer.hpp output_handler.hpp
	g++ -fsanitize=address -g -c main.cpp

clean :
	rm -f *.o lex.yy.* *.tab.h *.tab.c *.output *.log *.ast *.hh *.toks *.tac *.rtl *.spim exp? *.tab.cc a.out sclp

.PHONY: all clean gdb test
