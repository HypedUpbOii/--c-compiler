#pragma once
#include "register.hpp"
#include <memory>
#include <ostream>
#include <vector>

class ASM_Opd {
  public:
    OpdType opd_type;
    virtual std::string get_name() = 0;
    virtual ~ASM_Opd() = default;
};

class ASM_Double_Const_Opd : public ASM_Opd {
  private:
    double value;

  public:
    ASM_Double_Const_Opd(double val);
    std::string get_name() override;
};

class ASM_Int_Const_Opd : public ASM_Opd {
  private:
    int value;

  public:
    ASM_Int_Const_Opd(int val);
    std::string get_name() override;
};

class ASM_Label_Opd : public ASM_Opd {
  private:
    unsigned int label_num;
    std::string label_name;

  public:
    ASM_Label_Opd(unsigned int num);
    ASM_Label_Opd(std::string label_name);
    std::string get_name() override;
};

class ASM_Mem_Opd : public ASM_Opd {
  private:
    std::string name;
    int fp_offset;
    RegisterDescriptor *rd;

  public:
    ASM_Mem_Opd(int offset, RegisterDescriptor *r);
    ASM_Mem_Opd(std::string);
    std::string get_name() override;
};

class ASM_Register_Opd : public ASM_Opd {
  public:
    RegisterDescriptor *reg_desc;
    ASM_Register_Opd(RegisterDescriptor *r);
    std::string get_name() override;
};

class ASM_String_Const_Opd : public ASM_Opd {
  private:
    unsigned int string_num;
    std::string value;

  public:
    ASM_String_Const_Opd(unsigned int num, std::string s);
    std::string get_name() override;
};

// --------------------------------------------------------------

class ASM_Stmt {
  public:
    virtual void print(std::ostream &) = 0;
    virtual ~ASM_Stmt() = default;
};

class Compute_ASM_Stmt : public ASM_Stmt {
  protected:
    std::shared_ptr<ASM_Opd> result;
    std::shared_ptr<ASM_Opd> oper1;
    std::shared_ptr<ASM_Opd> oper2;
    bool isfloat;
};

class Arithmetic_ASM_Stmt : public Compute_ASM_Stmt {
    ArithmeticOperator oper; // add, sub, mul, div

  public:
    Arithmetic_ASM_Stmt(std::shared_ptr<ASM_Register_Opd> res,
                        std::shared_ptr<ASM_Register_Opd> op1,
                        std::shared_ptr<ASM_Opd> op2, ArithmeticOperator opr);
    void print(std::ostream &) override;
};

class Boolean_ASM_Stmt : public Compute_ASM_Stmt {
    BooleanOperator oper; // and, or

  public:
    Boolean_ASM_Stmt(std::shared_ptr<ASM_Register_Opd> res,
                     std::shared_ptr<ASM_Register_Opd> op1,
                     std::shared_ptr<ASM_Register_Opd> op2,
                     BooleanOperator opr);
    void print(std::ostream &) override;
};

class Relational_ASM_Stmt : public Compute_ASM_Stmt {
    RelationalOperator oper; // sgt, slt, sle, sge, seq, sne
                             // c.lt.d, c.le.d, c.eq.d
  public:
    Relational_ASM_Stmt(std::shared_ptr<ASM_Register_Opd> res,
                        std::shared_ptr<ASM_Register_Opd> op1,
                        std::shared_ptr<ASM_Register_Opd> op2,
                        RelationalOperator opr);
    Relational_ASM_Stmt(std::shared_ptr<ASM_Register_Opd> op1,
                        std::shared_ptr<ASM_Register_Opd> op2,
                        RelationalOperator opr);
    void print(std::ostream &) override;
};

class UMinus_ASM_Stmt : public Compute_ASM_Stmt { // neg, neg.d
  public:
    UMinus_ASM_Stmt(std::shared_ptr<ASM_Register_Opd> res,
                    std::shared_ptr<ASM_Register_Opd> op);
    void print(std::ostream &) override;
};

class Not_ASM_Stmt : public Compute_ASM_Stmt { // xori
  public:
    Not_ASM_Stmt(std::shared_ptr<ASM_Register_Opd> res,
                 std::shared_ptr<ASM_Register_Opd> op);
    void print(std::ostream &) override;
};

class Control_Flow_ASM_Stmt : public ASM_Stmt {
  protected:
    std::shared_ptr<ASM_Label_Opd> label;
    std::shared_ptr<ASM_Register_Opd> reg;

  public:
    virtual ~Control_Flow_ASM_Stmt() = default;
};

class Call_ASM_Stmt : public Control_Flow_ASM_Stmt { // jal func
  public:
    Call_ASM_Stmt(std::shared_ptr<ASM_Label_Opd>);
    void print(std::ostream &) override;
};

class Goto_ASM_Stmt : public Control_Flow_ASM_Stmt { // j label
  public:
    Goto_ASM_Stmt(std::shared_ptr<ASM_Label_Opd>);
    void print(std::ostream &) override;
};

class If_Goto_ASM_Stmt : public Control_Flow_ASM_Stmt { // bgz
  public:
    If_Goto_ASM_Stmt(std::shared_ptr<ASM_Register_Opd>,
                     std::shared_ptr<ASM_Label_Opd>);
    void print(std::ostream &) override;
};

class Jump_Reg_ASM_Stmt : public Control_Flow_ASM_Stmt { // j ra
  public:
    Jump_Reg_ASM_Stmt(std::shared_ptr<ASM_Register_Opd>);
    void print(std::ostream &) override;
};

class Label_ASM_Stmt : public ASM_Stmt {
  private:
    std::shared_ptr<ASM_Label_Opd> label;

  public:
    Label_ASM_Stmt(std::shared_ptr<ASM_Label_Opd>);
    void print(std::ostream &) override;
};

class Transfer_ASM_Stmt : public ASM_Stmt {
  protected:
    bool isFloat;
    std::shared_ptr<ASM_Opd> source;
    std::shared_ptr<ASM_Opd> dest;
};

class Move_If_ASM_Stmt : public Transfer_ASM_Stmt {
    unsigned int flag;
    bool ist;

  public:
    Move_If_ASM_Stmt(std::shared_ptr<ASM_Register_Opd>,
                     std::shared_ptr<ASM_Register_Opd>, unsigned int fl,
                     bool is);
    void print(std::ostream &) override;
};

class Move_ASM_Stmt : public Transfer_ASM_Stmt {
  public:
    Move_ASM_Stmt(std::shared_ptr<ASM_Register_Opd>,
                  std::shared_ptr<ASM_Register_Opd>);
    void print(std::ostream &) override;
};

class Load_Int_Immediate_ASM_Stmt : public Transfer_ASM_Stmt {
  public:
    Load_Int_Immediate_ASM_Stmt(std::shared_ptr<ASM_Register_Opd>,
                                std::shared_ptr<ASM_Int_Const_Opd>);
    void print(std::ostream &) override;
};

class Load_Double_Immediate_ASM_Stmt : public Transfer_ASM_Stmt {
  public:
    Load_Double_Immediate_ASM_Stmt(std::shared_ptr<ASM_Register_Opd>,
                                   std::shared_ptr<ASM_Double_Const_Opd>);
    void print(std::ostream &) override;
};

class Load_Mem_ASM_Stmt : public Transfer_ASM_Stmt {
  public:
    Load_Mem_ASM_Stmt(std::shared_ptr<ASM_Register_Opd>,
                      std::shared_ptr<ASM_Mem_Opd>);
    void print(std::ostream &) override;
};

class Load_Address_ASM_Stmt : public Transfer_ASM_Stmt {
  public:
    Load_Address_ASM_Stmt(std::shared_ptr<ASM_Register_Opd>,
                          std::shared_ptr<ASM_String_Const_Opd>);
    Load_Address_ASM_Stmt(std::shared_ptr<ASM_Register_Opd>,
                          std::shared_ptr<ASM_Mem_Opd>);
    void print(std::ostream &) override;
};

class Store_Mem_ASM_Stmt : public Transfer_ASM_Stmt {
  public:
    Store_Mem_ASM_Stmt(std::shared_ptr<ASM_Mem_Opd>,
                       std::shared_ptr<ASM_Register_Opd>);
    void print(std::ostream &) override;
};

class Syscall_ASM_Stmt : public ASM_Stmt { // just syscall
  public:
    Syscall_ASM_Stmt();
    void print(std::ostream &) override;
};

class SPIM {
    std::vector<std::shared_ptr<ASM_Stmt>> assembly_code;

  public:
    void addSPIM(std::shared_ptr<ASM_Stmt> code);
    void print(std::ostream &);
};
