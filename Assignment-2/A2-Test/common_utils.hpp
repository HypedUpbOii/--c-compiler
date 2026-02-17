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

enum class DataType {INT, FLOAT, BOOL, STRING, VOID};

enum class BinaryOperator {
    PLUS,
    MINUS,
    MULT,
    DIVIDE,

    LESS_THAN,
    LESS_THAN_EQUAL,
    GREATER_THAN,
    GREATER_THAN_EQUAL,
    EQUAL,
    NOT_EQUAL,

    OR,
    AND
};

enum class UnaryOperator {
    NOT,
    UMINUS
};