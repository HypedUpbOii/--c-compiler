#pragma once
#include "common_utils.hpp"
#include "symbol_table.hpp"
#include "tac.hpp"
#include <iomanip>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

// abstract class
class Ast {
  public:
    virtual void validateNode() = 0;
    virtual void printTree(std::ostream &, int) = 0;
    virtual void generateTAC(TAC &) = 0;
    virtual ~Ast() = default;
};

// Expression
// abstract class
class Expression_Ast : public Ast {
  public:
    TAC_Opd *place;               // delete in destructor
    std::vector<TAC_Stmt *> code; // clear vector after used
    DataType exprType; // type set in validateNode() except for literals

    ~Expression_Ast();
};

// Base Expression - Function calls, variables, constants
// abstract class
class Base_Expr_Ast : public Expression_Ast {
  public:
    virtual ~Base_Expr_Ast() = default;
};

class Function_Call_Ast : public Base_Expr_Ast {
  public:
    // L5
};

class Name_Expr_Ast : public Base_Expr_Ast { // variable
  public:
    std::string name;
    SymbolTableEntry *steEntry;

    Name_Expr_Ast(std::string, SymbolTableEntry *);
    void validateNode() override;
    void printTree(std::ostream &, int) override;
    void generateTAC(TAC &) override;
};

template <typename T>
class Literal_Expr_Ast
    : public Base_Expr_Ast { // int, float and string constants
  public:
    T value;

    Literal_Expr_Ast(T);
    void validateNode() override;
    void printTree(std::ostream &, int) override;
    void generateTAC(TAC &) override;
};

// Binary Expressions
// abstract class
class Binary_Expr_Ast : public Expression_Ast {
  public:
    std::unique_ptr<Expression_Ast> leftOp;
    std::unique_ptr<Expression_Ast> rightOp;

    void printChildren(std::ostream &, int);
};

class Array_Access_Expr_Ast : public Binary_Expr_Ast {
  public:
    // L6
};

class Boolean_Expr_Ast : public Binary_Expr_Ast { // boolean operators - or, and
  public:
    BooleanOperator op;

    Boolean_Expr_Ast(std::unique_ptr<Expression_Ast>, BooleanOperator,
                     std::unique_ptr<Expression_Ast>);
    void validateNode() override;
    void printTree(std::ostream &, int) override;
    void generateTAC(TAC &) override;
};

class Arithmetic_Expr_Ast
    : public Binary_Expr_Ast { // all operations - +, -, /, *
  public:
    ArithmeticOperator op;

    Arithmetic_Expr_Ast(std::unique_ptr<Expression_Ast>, ArithmeticOperator,
                        std::unique_ptr<Expression_Ast>);
    void validateNode() override;
    void printTree(std::ostream &, int) override;
    void generateTAC(TAC &) override;
};

class Relational_Expr_Ast : public Binary_Expr_Ast { // all comparisons
  public:
    RelationalOperator op;

    Relational_Expr_Ast(std::unique_ptr<Expression_Ast>, RelationalOperator,
                        std::unique_ptr<Expression_Ast>);
    void validateNode() override;
    void printTree(std::ostream &, int) override;
    void generateTAC(TAC &) override;
};

// Ternary Expressions
class Ternary_Expr_Ast : public Expression_Ast { // maybe ? idk : hehe
  public:
    std::unique_ptr<Expression_Ast> condition;
    std::unique_ptr<Expression_Ast> trueExpr;
    std::unique_ptr<Expression_Ast> falseExpr;

    Ternary_Expr_Ast(std::unique_ptr<Expression_Ast>,
                     std::unique_ptr<Expression_Ast>,
                     std::unique_ptr<Expression_Ast>);
    void validateNode() override;
    void printTree(std::ostream &, int) override;
    void generateTAC(TAC &) override;
};

// Unary Expressions
// abstract class
class Unary_Expr_Ast : public Expression_Ast {
  public:
    virtual ~Unary_Expr_Ast() = default;
};

class Address_Expr_Ast : public Unary_Expr_Ast { // &a
  public:
    // L6
};

class Pointer_Deref_Expr_Ast : public Unary_Expr_Ast { // *a
  public:
    // L6
};

class UMinus_Expr_Ast : public Unary_Expr_Ast {
  public:
    std::unique_ptr<Expression_Ast> operand;

    UMinus_Expr_Ast(std::unique_ptr<Expression_Ast>);
    void validateNode() override;
    void printTree(std::ostream &, int) override;
    void generateTAC(TAC &) override;
};

class Not_Expr_Ast : public Unary_Expr_Ast { // !(bool)
  public:
    std::unique_ptr<Expression_Ast> operand;

    Not_Expr_Ast(std::unique_ptr<Expression_Ast>);
    void validateNode() override;
    void printTree(std::ostream &, int) override;
    void generateTAC(TAC &) override;
};

// Statements
// abstract class
class Statement_Ast : public Ast {
  public:
    virtual ~Statement_Ast() = default;
};

class Assignment_Stmt_Ast : public Statement_Ast {
  public:
    std::unique_ptr<Name_Expr_Ast> target;
    std::unique_ptr<Expression_Ast> value;

    Assignment_Stmt_Ast(std::unique_ptr<Name_Expr_Ast>,
                        std::unique_ptr<Expression_Ast>);
    void validateNode() override;
    void printTree(std::ostream &, int) override;
    void generateTAC(TAC &) override;
};

class Iteration_Stmt_Ast : public Statement_Ast { // while, do-while statments
  public:
    // L4
};

class Read_Stmt_Ast : public Statement_Ast {
  public:
    std::unique_ptr<Name_Expr_Ast> target;

    Read_Stmt_Ast(std::unique_ptr<Name_Expr_Ast>);
    void validateNode() override;
    void printTree(std::ostream &, int) override;
    void generateTAC(TAC &) override;
};

class Return_Stmt_Ast : public Statement_Ast {
  public:
    // L5
};

class Selection_Stmt_Ast : public Statement_Ast { // if, if-else statements
  public:
    // L4
};

class Sequence_Stmt_Ast : public Statement_Ast { // block statements
  public:
    // L4
};

class Write_Stmt_Ast : public Statement_Ast {
  public:
    std::unique_ptr<Expression_Ast> target;

    Write_Stmt_Ast(std::unique_ptr<Expression_Ast>);
    void validateNode() override;
    void printTree(std::ostream &, int) override;
    void generateTAC(TAC &) override;
};
