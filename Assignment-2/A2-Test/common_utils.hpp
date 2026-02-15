#pragma once
#include <string>

struct Arguments {
    std::string input_file = "";
    bool stop_after_scan = false;
    bool stop_after_parse = false;
    bool show_tokens = false;
    bool show_ast = false;
    bool demo_mode = false;
};