#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include <streambuf>
#include "common_utils.hpp"

class NullBuffer : public std::streambuf {
protected:
    int overflow(int c) override;
};

class NullStream : public std::ostream {
private:
    NullBuffer buffer;

public:
    NullStream();
};

class OutputHandler {
private:
    bool demo_mode;
    std::string toks_file;
    std::string ast_file;
    std::string tac_file;

    std::stringstream token_buffer;
    std::stringstream ast_buffer;
    std::stringstream tac_buffer;

    std::ostream* token_stream;
    std::ostream* ast_stream;
    std::ostream* tac_stream;

public:
    OutputHandler(const Arguments& args);

    std::ostream& tokenStream();
    std::ostream& astStream();
    std::ostream& tacStream();

    void commitTokens();
    void commitAst();
    void commitTac();
};
