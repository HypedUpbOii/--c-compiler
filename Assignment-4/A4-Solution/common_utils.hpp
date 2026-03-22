#pragma once
#include <array>
#include <iostream>
#include <string>
#include <vector>

struct Arguments {
    std::string input_file = "";
    bool stop_after_scan = false;
    bool stop_after_parse = false;
    bool stop_after_ast = false;
    bool stop_after_tac = false;
    bool show_tokens = false;
    bool show_ast = false;
    bool show_tac = false;
    bool show_rtl = false;
    bool demo_mode = false;
};

enum class BaseType : int { INT, FLOAT, BOOL, STRING, VOID };

class DataType {
  public:
    BaseType base;
    int pointer_level;
    std::vector<int> array_dimensions;

    DataType(const BaseType & = BaseType::VOID, int = 0,
             const std::vector<int> & = std::vector<int>());
    bool operator==(DataType);
    bool operator!=(DataType);
    bool operator==(BaseType);
    bool operator!=(BaseType);
    DataType &operator=(const DataType &);
    DataType &operator=(const BaseType &);
};

std::string type_to_string(DataType t);

enum class BooleanOperator : int { OR, AND };

std::string bool_op_to_string(BooleanOperator op);
std::string bool_op_to_symbol(BooleanOperator op);

enum class ArithmeticOperator : int { PLUS, MINUS, MULT, DIVIDE };

std::string arith_op_to_string(ArithmeticOperator op);
std::string arith_op_to_symbol(ArithmeticOperator op);

enum class RelationalOperator : int {
    LESS_THAN,
    LESS_THAN_EQUAL,
    GREATER_THAN,
    GREATER_THAN_EQUAL,
    EQUAL,
    NOT_EQUAL
};

std::string rel_op_to_string(RelationalOperator op);
std::string rel_op_to_symbol(RelationalOperator op);

enum class UnaryOperator : int { UMINUS, NOT };

std::string unary_op_to_string(UnaryOperator op);
std::string unary_op_to_symbol(UnaryOperator op);

void exit_with_err_msg(std::string msg);
