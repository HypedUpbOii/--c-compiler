#pragma once
#include "common_utils.hpp"
#include "symbol_table.hpp"
#include <string>
#include <ostream>
#include <vector>
#include <set>
#include <iomanip>
#include <memory>

class ASTNode {
public:
    virtual bool validateNode();
    virtual void printTree(std::ostream&, int);
    virtual ~ASTNode() = default;
};

class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<FunctionNode>> funcs;
    bool hasFuncDecl;
    SymbolTable* global;

    ProgramNode();
    void setSymbolTable(SymbolTable*);
    bool validateNode() override;
    virtual void printTree(std::ostream&, int) override;
};

class FunctionNode : public ASTNode {
public:
    DataType returnType;
    std::vector<std::pair<std::string, DataType>> parameters;
    std::string name;
    std::vector<std::unique_ptr<StmtNode>> statements;
    SymbolTable* local;

    FunctionNode();
    void setSymbolTable(SymbolTable*);
    bool validateNode() override;
    virtual void printTree(std::ostream&, int) override;
};

class StmtNode : public ASTNode {
public:
    virtual ~StmtNode() = default;
};

class AssignStmtNode : public StmtNode {
public:
    std::unique_ptr<VariableExprNode> target;
    std::unique_ptr<ExprNode> value;

    AssignStmtNode(std::unique_ptr<VariableExprNode>, std::unique_ptr<ExprNode>);
    bool validateNode() override;
    virtual void printTree(std::ostream&, int) override;
};

class ReadStmtNode : public StmtNode {
public:
    std::unique_ptr<VariableExprNode> target;

    ReadStmtNode(std::unique_ptr<VariableExprNode>);
    bool validateNode() override;
    virtual void printTree(std::ostream&, int) override;
};

class PrintStmtNode : public StmtNode {
public:
    std::unique_ptr<ExprNode> target;

    PrintStmtNode(std::unique_ptr<ExprNode>);
    bool validateNode() override;
    virtual void printTree(std::ostream&, int) override;
};

class ExprNode : public ASTNode {
public:
    DataType exprType; // to be set in validateNode() except for literals
    virtual ~ExprNode() = default;
};

class BinaryExprNode : public ExprNode {
public:
    BinaryOperator op;
    std::unique_ptr<ExprNode> leftOp;
    std::unique_ptr<ExprNode> rightOp;

    BinaryExprNode::BinaryExprNode(BinaryOperator, std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>);
    bool validateNode() override;
    virtual void printTree(std::ostream&, int) override;
};

class UnaryExprNode : public ExprNode {
public:
    UnaryOperator op;
    std::unique_ptr<ExprNode> operand;

    UnaryExprNode(UnaryOperator, std::unique_ptr<ExprNode>);
    bool validateNode() override;
    virtual void printTree(std::ostream&, int) override;
};

class TernaryExprNode : public ExprNode {
public:
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<ExprNode> trueExpr;
    std::unique_ptr<ExprNode> falseExpr;

    TernaryExprNode(std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>);
    bool validateNode() override;
    virtual void printTree(std::ostream&, int) override;
};

class LiteralExprNode : public ExprNode {
public:
    virtual ~LiteralExprNode() = default;
    bool validateNode() override;
};

class IntExprNode : public LiteralExprNode {
public:
    int value;

    IntExprNode(int);
    virtual void printTree(std::ostream&, int) override;
};

class FloatExprNode : public LiteralExprNode {
public:
    float value;

    FloatExprNode(float);
    virtual void printTree(std::ostream&, int) override;
};

class StringExprNode : public LiteralExprNode {
public:
    std::string value;

    StringExprNode(std::string);
    virtual void printTree(std::ostream&, int) override;
};

class VariableExprNode : public ExprNode {
public:
    std::string name;
    SymbolTableEntry* steEntry;

    VariableExprNode(std::string, SymbolTableEntry*);
    bool validateNode();
    virtual void printTree(std::ostream&, int) override;
};
