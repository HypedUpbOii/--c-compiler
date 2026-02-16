#pragma once
#include "parser.tab.hh"
#include <iostream>
#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif

using namespace std;

class Lexer : public yyFlexLexer {
private:
    string input_file;
    string next_token;
    std::ostream& outstream;

    parser::location loc;
public:
    Lexer(std::istream* in, std::string, std::ostream& stream);

    parser::Parser::symbol_type nextToken();

    void write_token(const string& type);
    void lexerror();
};
