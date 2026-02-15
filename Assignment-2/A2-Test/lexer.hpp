#pragma once
#include <iostream>
#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif
#include "parser.tab.hh"

using namespace std;

class Lexer : public yyFlexLexer {
private:
    string input_file;
    string next_token;
    std::ostream& outstream;

    location_type loc;
public:
    Lexer(std::istream* in, std::ostream& stream);
    symbol_type yylex();

    void write_token(const string& type);
    void lexerror();
};
