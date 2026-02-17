#pragma once
#include "parser.tab.hh"
#include <iostream>
#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif

class Lexer : public yyFlexLexer {
private:
std::ostream& outstream;
parser::location loc;

public:
    Lexer(std::istream* in, std::string, std::ostream& stream);
    std::string input_file;
    std::string next_token;

    parser::Parser::symbol_type nextToken();

    void write_token(const std::string& type);
    void lexerror();
};
