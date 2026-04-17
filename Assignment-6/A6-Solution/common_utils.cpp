#include "common_utils.hpp"
#include <array>
#include <iostream>

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

int DataType::size() const {
    if (pointer_level > 0)
        return 4;
    else if (array_dimensions.empty()) {
        return (base == BaseType::FLOAT) ? 8 : 4;
    }

    int basesize = (base == BaseType::FLOAT) ? 8 : 4;
    for (auto sz : array_dimensions) {
        basesize *= sz;
    }
    return basesize;
}

bool DataType::needFloatReg() const { return (*this == BaseType::FLOAT); }

bool param_type_equals(DataType a, DataType b) {
    if (a.base != b.base || a.pointer_level != b.pointer_level ||
        a.array_dimensions.size() != b.array_dimensions.size()) {
        return false;
    }

    for (int i = 1; i < a.array_dimensions.size(); ++i) {
        if (a.array_dimensions[i] != b.array_dimensions[i]) {
            return false;
        }
    }
    return true;
}

RelationalOperator anti_op(RelationalOperator op) {
    switch (op) {
    case RelationalOperator::GREATER_THAN:
        return RelationalOperator::LESS_THAN_EQUAL;
    case RelationalOperator::GREATER_THAN_EQUAL:
        return RelationalOperator::LESS_THAN;
    case RelationalOperator::NOT_EQUAL:
        return RelationalOperator::EQUAL;
    default:
        exit_with_err_msg("kys");
        return RelationalOperator::EQUAL;
    }
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

static std::array<std::string, 48> reg_symbols = {
    "none", "empty", "v0",  "v1",

    "a0",   "a1",    "a2",  "a3",

    "t0",   "t1",    "t2",  "t3",  "t4",  "t5",  "t6",  "t7",  "t8",  "t9",

    "s0",   "s1",    "s2",  "s3",  "s4",  "s5",  "s6",  "s7",

    "mfc",  "mtc",

    "f0",   "f2",    "f4",  "f6",  "f8",  "f10", "f12", "f14", "f16", "f18",
    "f20",  "f22",   "f24", "f26", "f28", "f30",

    "gp",   "sp",    "fp",  "ra",
};

std::string reg_to_symbols(Register r) {
    return reg_symbols.at(static_cast<int>(r));
}

static std::array<std::string, 4> arith_op_rtl_strings = {"add", "sub", "mul",
                                                          "div"};
static std::array<std::string, 6> rel_op_rtl_strings = {"slt", "sle", "sgt",
                                                        "sge", "seq", "sne"};
static std::array<std::string, 2> bool_op_rtl_strings = {"or", "and"};

std::string arith_op_to_rtl_string(ArithmeticOperator op) {
    return arith_op_rtl_strings.at(static_cast<int>(op));
}

std::string rel_op_to_rtl_string(RelationalOperator op) {
    return rel_op_rtl_strings.at(static_cast<int>(op));
}

std::string bool_op_to_rtl_string(BooleanOperator op) {
    return bool_op_rtl_strings.at(static_cast<int>(op));
}
