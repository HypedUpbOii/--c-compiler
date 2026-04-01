#pragma once
#include "common_utils.hpp"
#include "symbol_table.hpp"
#include "rtl.hpp"
#include "tac_operand.hpp"
#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------

class TAC_Stmt {
  protected:
    // these pointers are "owned" by the ast expr node and deleted by them
    std::shared_ptr<TAC_Opd> result;
    std::shared_ptr<TAC_Opd> oper1;
    std::shared_ptr<TAC_Opd> oper2;

  public:
    virtual ~TAC_Stmt() = default;
    virtual void print(std::ostream &) = 0;
    virtual void generateRTL(RTL &) = 0;
};

class Asgn_TAC_Stmt : public TAC_Stmt {
  public:
    Asgn_TAC_Stmt(std::shared_ptr<TAC_Opd> dest, std::shared_ptr<TAC_Opd> src);
    void print(std::ostream &) override;
    void generateRTL(RTL & __rtl) override;
};

class Call_TAC_Stmt : public TAC_Stmt {
  public:
    Call_TAC_Stmt(std::shared_ptr<TAC_Opd> func);
    void print(std::ostream &) override;
    void generateRTL(RTL & __rtl) override;
};

class Compute_TAC_Stmt : public TAC_Stmt {
  public:
    virtual ~Compute_TAC_Stmt() = default;
};

class Bool_Comp_TAC_Stmt : public Compute_TAC_Stmt {
  private:
    BooleanOperator op;

  public:
    Bool_Comp_TAC_Stmt(std::shared_ptr<TAC_Opd> result, std::shared_ptr<TAC_Opd> oper1, BooleanOperator op,
                       std::shared_ptr<TAC_Opd> oper2);
    void print(std::ostream &) override;
    void generateRTL(RTL & __rtl) override;
};

class Arith_Comp_TAC_Stmt : public Compute_TAC_Stmt {
  private:
    ArithmeticOperator op;

  public:
    Arith_Comp_TAC_Stmt(std::shared_ptr<TAC_Opd> result, std::shared_ptr<TAC_Opd> oper1, ArithmeticOperator op,
                        std::shared_ptr<TAC_Opd> oper2);
    void print(std::ostream &) override;
    void generateRTL(RTL & __rtl) override;
};

class Rel_Comp_TAC_Stmt : public Compute_TAC_Stmt {
  private:
    RelationalOperator op;
    bool needfloat;

  public:
    Rel_Comp_TAC_Stmt(std::shared_ptr<TAC_Opd> result, std::shared_ptr<TAC_Opd> oper1, RelationalOperator op,
                      std::shared_ptr<TAC_Opd> oper2);
    void print(std::ostream &) override;
    void generateRTL(RTL & __rtl) override;
};

class Unary_Comp_TAC_Stmt : public Compute_TAC_Stmt {
  private:
    UnaryOperator op;

  public:
    Unary_Comp_TAC_Stmt(std::shared_ptr<TAC_Opd> result, UnaryOperator op, std::shared_ptr<TAC_Opd> oper);
    void print(std::ostream &) override;
    void generateRTL(RTL & __rtl) override;
};

class Goto_TAC_Stmt : public TAC_Stmt {
  public:
    Goto_TAC_Stmt(std::shared_ptr<Label_TAC_Opd>label);
    void print(std::ostream &) override;
    void generateRTL(RTL & __rtl) override;
};

class If_Goto_TAC_Stmt : public TAC_Stmt {
  public:
    If_Goto_TAC_Stmt(std::shared_ptr<TAC_Opd> cond, std::shared_ptr<Label_TAC_Opd>label);
    void print(std::ostream &) override;
    void generateRTL(RTL & __rtl) override;
};

class IO_TAC_Stmt : public TAC_Stmt {
  private:
    bool is_write;

  public:
    IO_TAC_Stmt(bool is_write, std::shared_ptr<TAC_Opd> oper);
    void print(std::ostream &) override;
    void generateRTL(RTL & __rtl) override;
};

class Label_TAC_Stmt : public TAC_Stmt {
  public:
    Label_TAC_Stmt(std::shared_ptr<Label_TAC_Opd>label);
    void print(std::ostream &) override;
    void generateRTL(RTL & __rtl) override;
};

class Return_TAC_Stmt : public TAC_Stmt {
  public:
    Return_TAC_Stmt(std::shared_ptr<Variable_TAC_Opd> stemp);
    void print(std::ostream &) override;
    void generateRTL(RTL & __rtl) override;
};

class TAC {
  private:
    std::vector<TAC_Stmt *> tac_code;
    unsigned int temp_number;
    unsigned int stemp_number;
    static unsigned int label_number;

  public:
    TAC();
    static std::shared_ptr<Label_TAC_Opd> getRetLabel(); // sclp's retarted implementation
    std::shared_ptr<Temporary_TAC_Opd> genNewTemporary(bool need_float = false);
    std::shared_ptr<Variable_TAC_Opd> genNewSTemporary(DataType dt, SymbolTable *local);
    std::shared_ptr<Label_TAC_Opd> genNewLabel();
    void addTACStatements(const std::vector<TAC_Stmt *> &stmts);
    void print(std::ostream &);
    void generateRTL(RTL &);
    bool isEmpty();
    ~TAC();
};
