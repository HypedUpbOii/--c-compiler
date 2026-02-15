#pragma once
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
public:
    Lexer(std::istream* in, std::ostream& stream);
    int yylex();

    void write_token(const string& type);
    void lexerror();
};
