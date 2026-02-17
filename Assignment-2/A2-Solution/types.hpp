#pragma once

enum class DataType {
    INT,
    FLOAT,
    BOOL,
    VOID,
    STRING
};

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
