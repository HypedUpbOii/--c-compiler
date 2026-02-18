#include "common_utils.hpp"

std::string type_to_string(DataType t) {
    switch(t) {
        case DataType::INT : return "<int>";
        case DataType::FLOAT : return "<float>";
        case DataType::BOOL : return "<bool>";
        case DataType::STRING : return "<string>";
        case DataType::VOID : return "<void>";
        default : return "unknown";
    }
}

std::string op_to_string(BinaryOperator op) {
    switch(op) {
        case BinaryOperator::PLUS : return "Plus";
        case BinaryOperator::MINUS : return "Minus";
        case BinaryOperator::MULT : return "Mult";
        case BinaryOperator::DIVIDE : return "Div";

        case BinaryOperator::LESS_THAN : return "LT";
        case BinaryOperator::LESS_THAN_EQUAL : return "LE";
        case BinaryOperator::GREATER_THAN : return "GT";
        case BinaryOperator::GREATER_THAN_EQUAL : return "GE";
        case BinaryOperator::EQUAL : return "EQ";
        case BinaryOperator::NOT_EQUAL : return "NE";

        case BinaryOperator::OR : return "OR";
        case BinaryOperator::AND : return "AND";
        default : return "kys";
    }
}
