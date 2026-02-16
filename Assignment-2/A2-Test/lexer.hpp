#pragma once
#include <iostream>
#include "parser.tab.hh"

using namespace std;

class Lexer {
private:
    string input_file;
    string next_token;
    std::ostream& outstream;

public:
    parser::location loc;
    Lexer(const std::string&, std::ostream& stream);

    void write_token(const string& type);
    void lexerror();
};
