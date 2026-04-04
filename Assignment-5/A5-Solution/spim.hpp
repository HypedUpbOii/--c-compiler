#pragma once
#include "common_utils.hpp"
#include <ostream>
#include <vector>

class ASM_Opd {
  public:
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

  public:
    ASM_Label_Opd(unsigned int num);
    std::string get_name() override;
};

class ASM_Mem_Opd : public ASM_Opd {
  private:
    int fp_offset;
    Register reg;

  public:
    ASM_Mem_Opd(int offset, Register r);
    std::string get_name() override;
};

class ASM_Register_Opd : public ASM_Opd {
  private:
    Register reg;

  public:
    ASM_Register_Opd(Register r);
    std::string get_name() override;
};

class ASM_String_Const_Opd : public ASM_Opd {
  private:
    unsigned int string_num;

  public:
    ASM_String_Const_Opd(unsigned int num);
    std::string get_name() override;
};

class ASM_Stmt {
  public:
    virtual void print(std::ostream &) = 0;
    virtual ~ASM_Stmt() = default;
};

class Compute_ASM_Stmt : public ASM_Stmt { // add, sub, mul, and, or, slt
};

class Control_Flow_ASM_Stmt : public ASM_Stmt {
  public:
    virtual ~Control_Flow_ASM_Stmt() = default;
};

class Call_ASM_Stmt : public Control_Flow_ASM_Stmt { // jal func
};

class Goto_ASM_Stmt : public Control_Flow_ASM_Stmt { // j label
};

class If_Goto_ASM_Stmt : public Control_Flow_ASM_Stmt { // beq, ble, blt
};

class Jump_Reg_ASM_Stmt : public Control_Flow_ASM_Stmt { // j ra
};

class Label_ASM_Stmt : public ASM_Stmt {};

class Move_ASM_Stmt : public ASM_Stmt {};

class Syscall_ASM_Stmt : public ASM_Stmt { // just syscall
};

class SPIM {
  public:
    std::vector<ASM_Stmt *> assembly_code;
    void addSPIM(std::vector<ASM_Stmt *> code);
    void addGlobal(BaseType b, std::string name);
    void addString(std::string val, std::string name);
    void print(std::ostream &);
};