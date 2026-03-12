#pragma once
#include "common_utils.hpp"
#include "symbol_table.hpp"
#include "tac.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <iomanip>
#include <memory>

class ASTNode {
public:
    virtual bool validateNode() = 0;
    virtual void printTree(std::ostream&, int) = 0;
    virtual void generateTAC(TAC&) = 0;
    virtual ~ASTNode() = default;
};

class ExprNode : public ASTNode {
public:
    // delete in destructor
    TAC_Opd* place;
    // clear vector after used
    std::vector<TAC_Stmt*> code;
    DataType exprType; // to be set in validateNode() except for literals
    ~ExprNode();
};

class VariableExprNode : public ExprNode {
public:
    std::string name;
    SymbolTableEntry* steEntry;

    VariableExprNode(std::string, SymbolTableEntry*);
    bool validateNode();
    void printTree(std::ostream&, int) override;
    void generateTAC(TAC&) override;
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
    void printTree(std::ostream&, int) override;
    void generateTAC(TAC&) override;
};

class FunctionNode {
private:
    TAC tac;
public:
    DataType returnType;
    std::vector<std::pair<std::string, DataType>> parameters;
    std::string name;
    std::vector<std::unique_ptr<StmtNode>> statements;
    SymbolTable* local;

    FunctionNode(DataType ret, 
        std::vector<std::pair<std::string, DataType>> params,
        std::string nam, 
        std::vector<std::unique_ptr<StmtNode>> stmts, 
        SymbolTable* loc);
    bool validateNode();
    bool checkFuncVarConflict(const std::vector<std::tuple<DataType, std::string, std::vector<DataType>>>&);
    void printTree(std::ostream&, int);
    void generateTAC();
    void printTAC(std::ostream&);
};

class ProgramNode {
public:
    std::vector<std::unique_ptr<FunctionNode>> funcs;
    std::vector<std::tuple<DataType, std::string, std::vector<DataType>>> func_decls;
    SymbolTable* global;

    ProgramNode();
    ~ProgramNode();
    void setSymbolTable(SymbolTable*);
    bool validateNode();
    void printTree(std::ostream&, int);
    void generateTAC();
    void printTAC(std::ostream&);
};

class ReadStmtNode : public StmtNode {
public:
    std::unique_ptr<VariableExprNode> target;

    ReadStmtNode(std::unique_ptr<VariableExprNode>);
    bool validateNode() override;
    void printTree(std::ostream&, int) override;
    void generateTAC(TAC&) override;
};

class PrintStmtNode : public StmtNode {
public:
    std::unique_ptr<ExprNode> target;

    PrintStmtNode(std::unique_ptr<ExprNode>);
    bool validateNode() override;
    void printTree(std::ostream&, int) override;
    void generateTAC(TAC&) override;
};

class BinaryExprNode : public ExprNode {
public:
    BinaryOperator op;
    std::unique_ptr<ExprNode> leftOp;
    std::unique_ptr<ExprNode> rightOp;

    BinaryExprNode(BinaryOperator, std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>);
    bool validateNode() override;
    void printTree(std::ostream&, int) override;
    void generateTAC(TAC&) override;
};

class UnaryExprNode : public ExprNode {
public:
    UnaryOperator op;
    std::unique_ptr<ExprNode> operand;

    UnaryExprNode(UnaryOperator, std::unique_ptr<ExprNode>);
    bool validateNode() override;
    void printTree(std::ostream&, int) override;
    void generateTAC(TAC&) override;
};

class TernaryExprNode : public ExprNode {
public:
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<ExprNode> trueExpr;
    std::unique_ptr<ExprNode> falseExpr;

    TernaryExprNode(std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>);
    bool validateNode() override;
    void printTree(std::ostream&, int) override;
    void generateTAC(TAC&) override;
};

class LiteralExprNode : public ExprNode {
public:
    bool validateNode() override;
    virtual ~LiteralExprNode() = default;
};

class IntExprNode : public LiteralExprNode {
public:
    int value;

    IntExprNode(int);
    void printTree(std::ostream&, int) override;
    void generateTAC(TAC&) override;
};

class FloatExprNode : public LiteralExprNode {
public:
    double value;

    FloatExprNode(double);
    void printTree(std::ostream&, int) override;
    void generateTAC(TAC&) override;
};

class StringExprNode : public LiteralExprNode {
public:
    std::string value;

    StringExprNode(std::string);
    void printTree(std::ostream&, int) override;
    void generateTAC(TAC&) override;
};
