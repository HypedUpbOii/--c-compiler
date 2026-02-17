#pragma once
#include "types.hpp"
#include "symbol_table.hpp"
#include <string>
#include <memory>

class ASTNode {
public:
    bool validateNode();
    bool printNode();
    virtual ~ASTNode() = default;
};

class ProgramNode : public ASTNode {

};

class StmtNode : public ASTNode {
public:
    virtual ~StmtNode() = default;
};

class AssignStmtNode : public StmtNode {
public:
    std::unique_ptr<VariableExprNode> target;
    std::unique_ptr<ExprNode> value;
};

class ReadStmtNode : public StmtNode {
public:
    std::unique_ptr<VariableExprNode> target;
};

class PrintStmtNode : public StmtNode {
public:
    std::unique_ptr<ExprNode> target;
};

class ExprNode : public ASTNode {
public:
    DataType exprType;
    virtual ~ExprNode() = default; 
};

class BinaryExprNode : public ExprNode {
public:
    BinaryOperator op;
    std::unique_ptr<ExprNode> leftOp;
    std::unique_ptr<ExprNode> rightOp;
};

class UnaryExprNode : public ExprNode {
public:
    UnaryOperator op;
    std::unique_ptr<ExprNode> operand;
};

class TernaryExprNode : public ExprNode {
public:
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<ExprNode> trueExpr;
    std::unique_ptr<ExprNode> falseExpr;
};

class LiteralExprNode : public ExprNode {
public:
    virtual ~LiteralExprNode() = default;
};

class IntExprNode : public LiteralExprNode {
public:
    int value;

    IntExprNode(int i) : value(i) {
        exprType = DataType::INT;
    }
};

class FloatExprNode : public LiteralExprNode {
public:
    float value;

    FloatExprNode(float f) : value(f) {
        exprType = DataType::FLOAT;
    }
};

class BoolExprNode : public LiteralExprNode {
public:
    bool value;

    BoolExprNode(bool b) : value(b) {
        exprType = DataType::BOOL;
    }
};

class StringExprNode : public LiteralExprNode {
public:
    std::string value;

    StringExprNode(std::string s) : value(s) {
        exprType = DataType::STRING;
    }
};

class VariableExprNode : public ExprNode {
public:
    std::string name;
    SymbolTableEntry* steEntry;

    VariableExprNode(std::string name, SymbolTableEntry* steEntry) {
        if (steEntry == nullptr) return;
        exprType = steEntry->get_type();
    }
};
