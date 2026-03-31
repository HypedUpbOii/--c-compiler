#pragma once
#include "common_utils.hpp"
#include "symbol_table.hpp"
#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class TAC_Opd {
  protected:
    OpdType opd_type;

  public:
    virtual std::string get_name() = 0;
    OpdType get_opd_type() const;
};

class Array_Acces_TAC_Opd : public TAC_Opd {
  public:
    // TODO: L6
};

class Double_Const_TAC_Opd : public TAC_Opd {
  private:
    double value;

  public:
    Double_Const_TAC_Opd(double val);
    std::string get_name() override;
    double get_value() const;
};

class Int_Const_TAC_Opd : public TAC_Opd {
  private:
    int value;

  public:
    Int_Const_TAC_Opd(int val);
    std::string get_name() override;
    int get_value() const;
};

class String_Const_TAC_Opd : public TAC_Opd {
  private:
    std::string value;

  public:
    String_Const_TAC_Opd(const std::string &val);
    std::string get_name() override;
};

class Label_TAC_Opd : public TAC_Opd {
  private:
    unsigned int label_num;

  public:
    Label_TAC_Opd(unsigned int num);
    std::string get_name() override;
    unsigned int get_label_num() const;
};

class Pointer_Deref_TAC_Opd : public TAC_Opd {
  public:
    // TODO: L6
};

class Temporary_TAC_Opd : public TAC_Opd {
  private:
    unsigned int temp_num;
    bool is_special;
    bool needfloat;
  public:
    Temporary_TAC_Opd(unsigned int num, bool is_special = false, bool need_float = false);
    std::string get_name() override;
    unsigned int get_temp_num() const;
    bool get_need_float() const;
};

class Variable_TAC_Opd : public TAC_Opd {
  private:
    SymbolTableEntry *symtab_entry;

  public:
    Variable_TAC_Opd(SymbolTableEntry *symtab_entry);
    std::string get_name() override;
    SymbolTableEntry * get_sym_tab_entry() const;
};

// ----------------------------------------------------------------------------
#include "rtl.hpp"

class TAC_Stmt {
  protected:
    // these pointers are "owned" by the corresponding ast expr node and deleted
    // by them
    TAC_Opd *result;
    TAC_Opd *oper1;
    TAC_Opd *oper2;

  public:
    virtual void print(std::ostream &) = 0;
    virtual void generateRTL(RTL &) = 0;
};

class Asgn_TAC_Stmt : public TAC_Stmt {
  public:
    Asgn_TAC_Stmt(TAC_Opd *dest, TAC_Opd *src);
    void print(std::ostream &) override;
    void generateRTL(RTL & __rtl) override;
};

class Call_TAC_Stmt : public TAC_Stmt {
  public:
    // TODO: L5
    // each function will have a label
    // just jump to that label
    // parameters are evaluated before calling
};

class Compute_TAC_Stmt : public TAC_Stmt {
  public:
    virtual ~Compute_TAC_Stmt() = default;
};

class Bool_Comp_TAC_Stmt : public Compute_TAC_Stmt {
  private:
    BooleanOperator op;

  public:
    Bool_Comp_TAC_Stmt(TAC_Opd *result, TAC_Opd *oper1, BooleanOperator op,
                       TAC_Opd *oper2);
    void print(std::ostream &) override;
    void generateRTL(RTL & __rtl) override;
};

class Arith_Comp_TAC_Stmt : public Compute_TAC_Stmt {
  private:
    ArithmeticOperator op;

  public:
    Arith_Comp_TAC_Stmt(TAC_Opd *result, TAC_Opd *oper1, ArithmeticOperator op,
                        TAC_Opd *oper2);
    void print(std::ostream &) override;
    void generateRTL(RTL & __rtl) override;
};

class Rel_Comp_TAC_Stmt : public Compute_TAC_Stmt {
  private:
    RelationalOperator op;
    bool needfloat;

  public:
    Rel_Comp_TAC_Stmt(TAC_Opd *result, TAC_Opd *oper1, RelationalOperator op,
                      TAC_Opd *oper2);
    void print(std::ostream &) override;
    void generateRTL(RTL & __rtl) override;
};

class Unary_Comp_TAC_Stmt : public Compute_TAC_Stmt {
  private:
    UnaryOperator op;

  public:
    Unary_Comp_TAC_Stmt(TAC_Opd *result, UnaryOperator op, TAC_Opd *oper);
    void print(std::ostream &) override;
    void generateRTL(RTL & __rtl) override;
};

class Goto_TAC_Stmt : public TAC_Stmt {
  public:
    Goto_TAC_Stmt(Label_TAC_Opd *label);
    void print(std::ostream &) override;
    void generateRTL(RTL & __rtl) override;
};

class If_Goto_TAC_Stmt : public TAC_Stmt {
  public:
    If_Goto_TAC_Stmt(TAC_Opd *cond, Label_TAC_Opd *label);
    void print(std::ostream &) override;
    void generateRTL(RTL & __rtl) override;
};

class IO_TAC_Stmt : public TAC_Stmt {
  private:
    bool is_write;

  public:
    IO_TAC_Stmt(bool is_write, TAC_Opd *oper);
    void print(std::ostream &) override;
    void generateRTL(RTL & __rtl) override;
};

class Label_TAC_Stmt : public TAC_Stmt {
  public:
    Label_TAC_Stmt(Label_TAC_Opd *label);
    void print(std::ostream &) override;
    void generateRTL(RTL & __rtl) override;
};

class NOP_TAC_Stmt : public TAC_Stmt {
  public:
    // TODO: Idk when
};

class Return_TAC_Stmt : public TAC_Stmt {
  public:
    // TODO: L5
    // store results in a stemp
    // then go back to frame pointer etc
};

class TAC {
  private:
    std::vector<TAC_Stmt *> tac_code;
    unsigned int temp_number;
    unsigned int stemp_number;
    static unsigned int label_number;

  public:
    TAC();
    Temporary_TAC_Opd *genNewTemporary(bool need_float = false);
    Temporary_TAC_Opd *genNewSTemporary();
    Label_TAC_Opd *genNewLabel();
    void addTACStatements(const std::vector<TAC_Stmt *> &stmts);
    void print(std::ostream &);
    void generateRTL(RTL &);
    bool isEmpty();
    ~TAC();
};
