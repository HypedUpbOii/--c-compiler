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

/*
AST
    Expresssion AST
        Base AST
            Function call
            Name expr
            Number expr <T> (float and int)
            String expr
        Binary Expr AST
            Array access
            Boolean expr
            Div expr
            Field access
            Minus expr
            Mult expr
            Plus expr
            Relational expr
        Ternary expr AST
            Conditional expr
        Unary expr
            UMinus
            Pointer dereference
            Address expr
    Statement AST
        Assignment statement
        Iteration statement
        Read statement
        Return statement
        Selection statement
        Sequence statement
        Write statement
*/

struct FunctionAST {
    SymbolTable* localSymTab;
    std::vector<std::unique_ptr<StatementAST>> statements;
};

struct Program {
    SymbolTable* globalSymTab;
    std::multimap<std::string, std::vector<DataType>> func_decls; // multimap for function overloading
    std::vector<FunctionAST> functions;
};

class AST {
public:
    virtual bool validateNode() = 0;
    virtual void printTree(std::ostream&, int) = 0;
    virtual void generateTAC(TAC&);
    void error(const std::string&);
    virtual ~AST() = default;
};

class ExpressionAST : public AST {
protected:
    DataType exprType; 
public:
    virtual void evaluateType() = 0;
    virtual ~ExpressionAST() = default;
};

class BaseAST : public ExpressionAST {
public:

};

class FunctionAST : public BaseAST {

};