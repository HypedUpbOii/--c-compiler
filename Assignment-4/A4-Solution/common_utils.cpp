#include "common_utils.hpp"

static std::array<std::string, 5> type_strings = {"<int>", "<float>", "<bool>",
                                                  "<string>", "<void>"};
std::string type_to_string(DataType t) {
    return type_strings.at(static_cast<int>(t.base));
}

DataType::DataType(const BaseType &t, int level, const std::vector<int> &dims)
    : base(t), pointer_level(level), array_dimensions(dims) {}

bool DataType::operator==(DataType t) const {
    return ((this->base == t.base) &&
            (this->pointer_level == t.pointer_level) &&
            (this->array_dimensions == t.array_dimensions));
}

bool DataType::operator!=(DataType t) const { return !(*this == t); }

bool DataType::operator==(BaseType t) const {
    return ((this->base == t) && (this->pointer_level == 0) &&
            this->array_dimensions.empty());
}

bool DataType::operator!=(BaseType t) const { return !(*this == t); }

DataType &DataType::operator=(const DataType &t) {
    if (this == &t) {
        return *this;
    }

    this->base = t.base;
    this->pointer_level = t.pointer_level;
    this->array_dimensions = t.array_dimensions;

    return *this;
}

DataType &DataType::operator=(const BaseType &t) {
    this->base = t;
    this->pointer_level = 0;
    this->array_dimensions.clear();

    return *this;
}

static std::array<std::string, 2> bool_op_strings = {"OR", "AND"};
std::string bool_op_to_string(BooleanOperator op) {
    return bool_op_strings.at(static_cast<int>(op));
}

static std::array<std::string, 4> arith_op_strings = {"Plus", "Minus", "Mult",
                                                      "Div"};
std::string arith_op_to_string(ArithmeticOperator op) {
    return arith_op_strings.at(static_cast<int>(op));
}

static std::array<std::string, 6> rel_op_strings = {"LT", "LE", "GT",
                                                    "GE", "EQ", "NE"};
std::string rel_op_to_string(RelationalOperator op) {
    return rel_op_strings.at(static_cast<int>(op));
}

static std::array<std::string, 2> bool_op_symbols = {"||", "&&"};
std::string bool_op_to_symbol(BooleanOperator op) {
    return bool_op_symbols.at(static_cast<int>(op));
}

static std::array<std::string, 4> arith_op_symbols = {"+", "-", "*", "/"};
std::string arith_op_to_symbol(ArithmeticOperator op) {
    return arith_op_symbols.at(static_cast<int>(op));
}

static std::array<std::string, 6> rel_op_symbols = {"<",  "<=", ">",
                                                    ">=", "==", "!="};
std::string rel_op_to_symbol(RelationalOperator op) {
    return rel_op_symbols.at(static_cast<int>(op));
}

static std::array<std::string, 2> unary_op_string = {"UMinus", "NOT"};
std::string unary_op_to_string(UnaryOperator op) {
    return unary_op_string.at(static_cast<int>(op));
}

static std::array<std::string, 2> unary_op_symbol = {"-", "!"};
std::string unary_op_to_symbol(UnaryOperator op) {
    return unary_op_symbol.at(static_cast<int>(op));
}

void exit_with_err_msg(std::string msg) {
    std::cerr << msg << std::endl;
    exit(1);
}
