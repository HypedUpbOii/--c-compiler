#pragma once
#include "tac.hpp"

// abstract class
class Ast {
  public:
    virtual void validateNode(SymbolTable *) = 0;
    virtual void printTree(std::ostream &, int) = 0;
    virtual ~Ast() = default;
};

// Expression
// abstract class
class Expression_Ast : public Ast {
  public:
    std::shared_ptr<TAC_Opd> place; // delete in destructor
    DataType exprType; // type set in validateNode() except for literals

    virtual ~Expression_Ast() = default;
    virtual std::vector<TAC_Stmt *> generateTAC(TAC &, SymbolTable *) = 0;
    virtual bool isLvalue();
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
    std::string name;
    std::vector<std::unique_ptr<Expression_Ast>> arguments;
    SymbolTableFunction *funcEntry;
    std::shared_ptr<TAC_Opd> func_call_place;

    Function_Call_Ast(std::string,
                      std::vector<std::unique_ptr<Expression_Ast>>);

    void validateNode(SymbolTable *) override;
    void printTree(std::ostream &, int) override;
    std::vector<TAC_Stmt *> generateTAC(TAC &, SymbolTable *) override;
};

class Name_Expr_Ast : public Base_Expr_Ast { // variable
  public:
    std::string name;
    SymbolTableEntry *steEntry;

    Name_Expr_Ast(std::string);
    void validateNode(SymbolTable *) override;
    void printTree(std::ostream &, int) override;
    std::vector<TAC_Stmt *> generateTAC(TAC &, SymbolTable *) override;
    bool isLvalue() override;
};

template <typename T>
class Literal_Expr_Ast
    : public Base_Expr_Ast { // int, float and string constants
  public:
    T value;

    Literal_Expr_Ast(T);
    void validateNode(SymbolTable *) override;
    void printTree(std::ostream &, int) override;
    std::vector<TAC_Stmt *> generateTAC(TAC &, SymbolTable *) override;
};

// Binary Expressions
// abstract class
class Binary_Expr_Ast : public Expression_Ast {
  public:
    std::unique_ptr<Expression_Ast> leftOp;
    std::unique_ptr<Expression_Ast> rightOp;

    virtual ~Binary_Expr_Ast() = default;
    void printChildren(std::ostream &, int);
};

class Array_Access_Expr_Ast : public Binary_Expr_Ast {
  public:
    // L6
    std::string name;
    SymbolTableEntry *steEntry;
    std::vector<std::unique_ptr<Expression_Ast>> dims;

    Array_Access_Expr_Ast(std::string,
                          std::vector<std::unique_ptr<Expression_Ast>> &);
    bool isLvalue() override;
};

class Boolean_Expr_Ast : public Binary_Expr_Ast { // bool operators - or, and
  public:
    BooleanOperator op;

    Boolean_Expr_Ast(std::unique_ptr<Expression_Ast>, BooleanOperator,
                     std::unique_ptr<Expression_Ast>);
    void validateNode(SymbolTable *) override;
    void printTree(std::ostream &, int) override;
    std::vector<TAC_Stmt *> generateTAC(TAC &, SymbolTable *) override;
};

class Arithmetic_Expr_Ast : public Binary_Expr_Ast { // operations +, -, /, *
  public:
    ArithmeticOperator op;

    Arithmetic_Expr_Ast(std::unique_ptr<Expression_Ast>, ArithmeticOperator,
                        std::unique_ptr<Expression_Ast>);
    void validateNode(SymbolTable *) override;
    void printTree(std::ostream &, int) override;
    std::vector<TAC_Stmt *> generateTAC(TAC &, SymbolTable *) override;
};

class Relational_Expr_Ast : public Binary_Expr_Ast { // all comparisons
  public:
    RelationalOperator op;

    Relational_Expr_Ast(std::unique_ptr<Expression_Ast>, RelationalOperator,
                        std::unique_ptr<Expression_Ast>);
    void validateNode(SymbolTable *) override;
    void printTree(std::ostream &, int) override;
    std::vector<TAC_Stmt *> generateTAC(TAC &, SymbolTable *) override;
};

// Ternary Expressions
class Ternary_Expr_Ast : public Expression_Ast {
  public:
    std::unique_ptr<Expression_Ast> condition;
    std::unique_ptr<Expression_Ast> trueExpr;
    std::unique_ptr<Expression_Ast> falseExpr;

    Ternary_Expr_Ast(std::unique_ptr<Expression_Ast>,
                     std::unique_ptr<Expression_Ast>,
                     std::unique_ptr<Expression_Ast>);
    void validateNode(SymbolTable *) override;
    void printTree(std::ostream &, int) override;
    std::vector<TAC_Stmt *> generateTAC(TAC &, SymbolTable *) override;
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
    std::unique_ptr<Expression_Ast> operand;

    Address_Expr_Ast(std::unique_ptr<Expression_Ast>);
};

class Pointer_Deref_Expr_Ast : public Unary_Expr_Ast { // *a
  public:
    // L6
    std::string name;
    SymbolTableEntry *steEntry;
    int pointerLevel;

    Pointer_Deref_Expr_Ast(std::string, int);
    bool isLvalue() override;
};

class UMinus_Expr_Ast : public Unary_Expr_Ast {
  public:
    std::unique_ptr<Expression_Ast> operand;

    UMinus_Expr_Ast(std::unique_ptr<Expression_Ast>);
    void validateNode(SymbolTable *) override;
    void printTree(std::ostream &, int) override;
    std::vector<TAC_Stmt *> generateTAC(TAC &, SymbolTable *) override;
};

class Not_Expr_Ast : public Unary_Expr_Ast {
  public:
    std::unique_ptr<Expression_Ast> operand;

    Not_Expr_Ast(std::unique_ptr<Expression_Ast>);
    void validateNode(SymbolTable *) override;
    void printTree(std::ostream &, int) override;
    std::vector<TAC_Stmt *> generateTAC(TAC &, SymbolTable *) override;
};

// Statements
// abstract class
class Statement_Ast : public Ast {
  public:
    virtual ~Statement_Ast() = default;
    virtual std::vector<TAC_Stmt *>
    generateTAC(TAC &, SymbolTable *, std::shared_ptr<Variable_TAC_Opd>,
                std::shared_ptr<Label_TAC_Opd>) = 0;
    virtual bool hasReturn();
};

class Assignment_Stmt_Ast : public Statement_Ast {
  public:
    std::unique_ptr<Expression_Ast> target;
    std::unique_ptr<Expression_Ast> value;

    Assignment_Stmt_Ast(std::unique_ptr<Expression_Ast>,
                        std::unique_ptr<Expression_Ast>);
    void validateNode(SymbolTable *) override;
    void printTree(std::ostream &, int) override;
    std::vector<TAC_Stmt *>
    generateTAC(TAC &, SymbolTable *, std::shared_ptr<Variable_TAC_Opd>,
                std::shared_ptr<Label_TAC_Opd>) override;
};

class Iteration_Stmt_Ast : public Statement_Ast { // (do) while statments
  public:
    std::unique_ptr<Expression_Ast> condition;
    std::unique_ptr<Statement_Ast> body;

    Iteration_Stmt_Ast(std::unique_ptr<Expression_Ast>,
                       std::unique_ptr<Statement_Ast>);
    void validateNode(SymbolTable *) override;
    bool hasReturn() override;
};

class While_Stmt_Ast : public Iteration_Stmt_Ast {
  public:
    using Iteration_Stmt_Ast::Iteration_Stmt_Ast;
    void printTree(std::ostream &, int) override;
    std::vector<TAC_Stmt *>
    generateTAC(TAC &, SymbolTable *, std::shared_ptr<Variable_TAC_Opd>,
                std::shared_ptr<Label_TAC_Opd>) override;
};

class Do_While_Stmt_Ast : public Iteration_Stmt_Ast {
  public:
    using Iteration_Stmt_Ast::Iteration_Stmt_Ast;
    void printTree(std::ostream &, int) override;
    std::vector<TAC_Stmt *>
    generateTAC(TAC &, SymbolTable *, std::shared_ptr<Variable_TAC_Opd>,
                std::shared_ptr<Label_TAC_Opd>) override;
};

class Read_Stmt_Ast : public Statement_Ast {
  public:
    std::unique_ptr<Name_Expr_Ast> target;

    Read_Stmt_Ast(std::unique_ptr<Name_Expr_Ast>);
    void validateNode(SymbolTable *) override;
    void printTree(std::ostream &, int) override;
    std::vector<TAC_Stmt *>
    generateTAC(TAC &, SymbolTable *, std::shared_ptr<Variable_TAC_Opd>,
                std::shared_ptr<Label_TAC_Opd>) override;
};

class Return_Stmt_Ast : public Statement_Ast {
  public:
    // L5
    std::unique_ptr<Expression_Ast> return_value;
    std::string funcName;

    Return_Stmt_Ast(std::unique_ptr<Expression_Ast>, std::string);
    void validateNode(SymbolTable *) override;
    void printTree(std::ostream &, int) override;
    std::vector<TAC_Stmt *>
    generateTAC(TAC &, SymbolTable *, std::shared_ptr<Variable_TAC_Opd>,
                std::shared_ptr<Label_TAC_Opd>) override;
    bool hasReturn() override;
};

class Selection_Stmt_Ast : public Statement_Ast { // if, if-else statements
  public:
    std::unique_ptr<Expression_Ast> condition;
    std::unique_ptr<Statement_Ast> then_stmt;
    std::unique_ptr<Statement_Ast> else_stmt;

    Selection_Stmt_Ast(std::unique_ptr<Expression_Ast>,
                       std::unique_ptr<Statement_Ast>,
                       std::unique_ptr<Statement_Ast>);
    void validateNode(SymbolTable *) override;
    void printTree(std::ostream &, int) override;
    std::vector<TAC_Stmt *>
    generateTAC(TAC &, SymbolTable *, std::shared_ptr<Variable_TAC_Opd>,
                std::shared_ptr<Label_TAC_Opd>) override;
    bool hasReturn() override;
};

class Sequence_Stmt_Ast : public Statement_Ast { // block statements
  public:
    std::vector<std::unique_ptr<Statement_Ast>> statement_list;

    Sequence_Stmt_Ast(std::vector<std::unique_ptr<Statement_Ast>> &);
    void validateNode(SymbolTable *) override;
    void printTree(std::ostream &, int) override;
    std::vector<TAC_Stmt *>
    generateTAC(TAC &, SymbolTable *, std::shared_ptr<Variable_TAC_Opd>,
                std::shared_ptr<Label_TAC_Opd>) override;
    bool hasReturn() override;
};

class Write_Stmt_Ast : public Statement_Ast {
  public:
    std::unique_ptr<Expression_Ast> target;

    Write_Stmt_Ast(std::unique_ptr<Expression_Ast>);
    void validateNode(SymbolTable *) override;
    void printTree(std::ostream &, int) override;
    std::vector<TAC_Stmt *>
    generateTAC(TAC &, SymbolTable *, std::shared_ptr<Variable_TAC_Opd>,
                std::shared_ptr<Label_TAC_Opd>) override;
};

class Call_Stmt_Ast : public Statement_Ast {
  public:
    // L5
    std::unique_ptr<Function_Call_Ast> func_call;

    Call_Stmt_Ast(std::unique_ptr<Function_Call_Ast>);
    void validateNode(SymbolTable *) override;
    void printTree(std::ostream &, int) override;
    std::vector<TAC_Stmt *>
    generateTAC(TAC &, SymbolTable *, std::shared_ptr<Variable_TAC_Opd>,
                std::shared_ptr<Label_TAC_Opd>) override;
};

// Storing ast for a function
class Function_Ast {
  private:
    TAC tac;
    RTL rtl;
    SPIM spim;

  public:
    DataType returnType;
    std::vector<std::pair<std::string, DataType>> parameters;
    std::string name;
    std::vector<std::unique_ptr<Statement_Ast>> statements;
    SymbolTable *local;

    Function_Ast(DataType ret,
                 std::vector<std::pair<std::string, DataType>> params,
                 std::string nam,
                 std::vector<std::unique_ptr<Statement_Ast>> stmts,
                 SymbolTable *loc, bool add_underscore = true);
    ~Function_Ast();
    void validateFunction();
    void printTree(std::ostream &);
    void generateTAC(std::shared_ptr<Label_TAC_Opd> ret_label);
    void printTAC(std::ostream &);
    void generateRTL();
    void printRTL(std::ostream &);
    void generateSPIM();
    void printSPIM(std::ostream &);
};

class Program {
  public:
    std::map<std::string, std::unique_ptr<Function_Ast>> funcs;
    std::map<std::string, std::shared_ptr<Label_TAC_Opd>> rets;
    std::vector<std::string> func_order;
    std::map<std::string, unsigned int> global_strings;
    std::vector<std::string> string_consts;
    std::vector<std::pair<DataType, std::string>> global_vars;
    SymbolTable *global;

    Program();
    ~Program();
    void addFuncDef(std::string, DataType);
    void addFunctions(std::vector<std::unique_ptr<Function_Ast>>);
    void addStringConst(std::string);
    void addGlobal(std::pair<DataType, std::vector<std::string>>);
    void validateProgram();
    void print(std::ostream &);
    void generateTAC();
    void printTAC(std::ostream &);
    void generateRTL();
    void printRTL(std::ostream &);
    void generateSPIM();
    void printSPIM(std::ostream &);
};
