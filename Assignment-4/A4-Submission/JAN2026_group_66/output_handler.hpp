#pragma once
#include "common_utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>

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
    std::string rtl_file;

    std::stringstream token_buffer;
    std::stringstream ast_buffer;
    std::stringstream tac_buffer;
    std::stringstream rtl_buffer;

    std::ostream *token_stream;
    std::ostream *ast_stream;
    std::ostream *tac_stream;
    std::ostream *rtl_stream;

  public:
    OutputHandler(const Arguments &args);

    std::ostream &tokenStream();
    std::ostream &astStream();
    std::ostream &tacStream();
    std::ostream &rtlStream();

    void commitTokens();
    void commitAst();
    void commitTac();
    void commitRtl();
};
