#pragma once
#include "spim.hpp"

class RTL_Opd {
  public:
    OpdType opd_type;
    virtual std::string get_name() = 0;
    virtual ~RTL_Opd() = default;
};

class RTL_Double_Const_Opd : public RTL_Opd {
  public:
    double value;
    RTL_Double_Const_Opd(double v);
    std::string get_name() override;
};

class RTL_Int_Const_Opd : public RTL_Opd {
  public:
    int value;
    RTL_Int_Const_Opd(int v);
    std::string get_name() override;
};

class RTL_Label_Opd : public RTL_Opd {
  public:
    unsigned int label_num;
    RTL_Label_Opd(int num);
    std::string get_name() override;
};

class RTL_Register_Opd : public RTL_Opd {
  public:
    RegisterDescriptor *reg_desc;
    RTL_Register_Opd(RegisterDescriptor *rd);
    std::string get_name() override;
};

class RTL_String_Const_Opd : public RTL_Opd {
  public:
    unsigned int string_num;
    std::string value;
    RTL_String_Const_Opd(unsigned int str_num, std::string s);
    std::string get_name() override;
    unsigned int get_string_num() const;
};

class RTL_Var_Opd : public RTL_Opd {
  public:
    SymbolTableEntry *entry;
    RTL_Var_Opd(SymbolTableEntry *e);
    std::string get_name() override;
};

// ---------------------------------------------------------------

class RTL_Stmt {
  protected:
    std::shared_ptr<RTL_Opd> result;
    std::shared_ptr<RTL_Opd> oper1;
    std::shared_ptr<RTL_Opd> oper2;

  public:
    virtual ~RTL_Stmt() = default;
    virtual void print(std::ostream &) = 0;
    virtual void generateSPIM(SPIM &, MachineDescriptor *md) = 0;
};

class Compute_RTL_Stmt : public RTL_Stmt {
  protected:
    bool isfloat;

  public:
    virtual ~Compute_RTL_Stmt() = default;
};

class Arithmetic_RTL_Stmt : public Compute_RTL_Stmt {
    ArithmeticOperator oper;

  public:
    Arithmetic_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> res,
                        std::shared_ptr<RTL_Register_Opd> op1,
                        std::shared_ptr<RTL_Register_Opd> op2,
                        ArithmeticOperator opr);
    void print(std::ostream &) override;
    void generateSPIM(SPIM &, MachineDescriptor *md) override;
};

class Boolean_RTL_Stmt : public Compute_RTL_Stmt {
    BooleanOperator oper;

  public:
    Boolean_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> res,
                     std::shared_ptr<RTL_Register_Opd> op1,
                     std::shared_ptr<RTL_Register_Opd> op2,
                     BooleanOperator opr);
    void print(std::ostream &) override;
    void generateSPIM(SPIM &, MachineDescriptor *md) override;
};

class Relational_RTL_Stmt : public Compute_RTL_Stmt {
    RelationalOperator oper;

  public:
    Relational_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> res,
                        std::shared_ptr<RTL_Register_Opd> op1,
                        std::shared_ptr<RTL_Register_Opd> op2,
                        RelationalOperator opr);
    Relational_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> op1,
                        std::shared_ptr<RTL_Register_Opd> op2,
                        RelationalOperator opr);
    void print(std::ostream &) override;
    void generateSPIM(SPIM &, MachineDescriptor *md) override;
};

class UMinus_RTL_Stmt : public Compute_RTL_Stmt {
  public:
    UMinus_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> res,
                    std::shared_ptr<RTL_Register_Opd> op);
    void print(std::ostream &) override;
    void generateSPIM(SPIM &, MachineDescriptor *md) override;
};

class Not_RTL_Stmt : public Compute_RTL_Stmt {
  public:
    Not_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> res,
                 std::shared_ptr<RTL_Register_Opd> op);
    void print(std::ostream &) override;
    void generateSPIM(SPIM &, MachineDescriptor *md) override;
};

class Control_Flow_RTL_Stmt : public RTL_Stmt {
  public:
    virtual ~Control_Flow_RTL_Stmt() = default;
};

class Call_RTL_Stmt : public Control_Flow_RTL_Stmt {
  private:
    SymbolTableFunction *entry;
    std::shared_ptr<RTL_Register_Opd> reg_opd;

  public:
    Call_RTL_Stmt(SymbolTableFunction *,
                  std::shared_ptr<RTL_Register_Opd> reg_op = nullptr);
    void print(std::ostream &) override;
    void generateSPIM(SPIM &, MachineDescriptor *md) override;
};

class Goto_RTL_Stmt : public Control_Flow_RTL_Stmt {
  public:
    Goto_RTL_Stmt(std::shared_ptr<RTL_Label_Opd> l);
    void print(std::ostream &) override;
    void generateSPIM(SPIM &, MachineDescriptor *md) override;
};

class If_Goto_RTL_Stmt : public Control_Flow_RTL_Stmt {
  public:
    // register -> oper1, label -> result
    If_Goto_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> r,
                     std::shared_ptr<RTL_Label_Opd> l);
    void print(std::ostream &) override;
    void generateSPIM(SPIM &, MachineDescriptor *md) override;
};

class Return_RTL_Stmt : public Control_Flow_RTL_Stmt {
  private:
    std::string func_name;

  public:
    Return_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> reg,
                    std::string func_nae);
    void print(std::ostream &) override;
    void generateSPIM(SPIM &, MachineDescriptor *md) override;
};

class Label_RTL_Stmt : public RTL_Stmt {
  public:
    Label_RTL_Stmt(std::shared_ptr<RTL_Label_Opd> l);
    void print(std::ostream &) override;
    void generateSPIM(SPIM &, MachineDescriptor *md) override;
};

class Transfer_RTL_Stmt : public RTL_Stmt {
    OpdType dest_type;
    OpdType src_type;
    bool isfloat;

  public:
    Transfer_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> dest,
                      std::shared_ptr<RTL_Register_Opd> src);
    Transfer_RTL_Stmt(std::shared_ptr<RTL_Var_Opd> dest,
                      std::shared_ptr<RTL_Register_Opd> src);
    Transfer_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> dest,
                      std::shared_ptr<RTL_Var_Opd> src);
    Transfer_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> dest,
                      std::shared_ptr<RTL_Int_Const_Opd> src);
    Transfer_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> dest,
                      std::shared_ptr<RTL_Double_Const_Opd> src);
    Transfer_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> dest,
                      std::shared_ptr<RTL_String_Const_Opd> src);

    void print(std::ostream &) override;
    void generateSPIM(SPIM &, MachineDescriptor *md) override;
};

class Read_RTL_Stmt : public RTL_Stmt {
  public:
    Read_RTL_Stmt();
    void print(std::ostream &) override;
    void generateSPIM(SPIM &, MachineDescriptor *md) override;
};

class Write_RTL_Stmt : public RTL_Stmt {
  public:
    Write_RTL_Stmt();
    void print(std::ostream &) override;
    void generateSPIM(SPIM &, MachineDescriptor *md) override;
};

class Mov_RTL_Stmt : public RTL_Stmt {
    unsigned int flag;
    bool movt;

  public:
    Mov_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> res,
                 std::shared_ptr<RTL_Register_Opd> opd, unsigned int _flag,
                 bool _movt);
    void print(std::ostream &) override;
    void generateSPIM(SPIM &, MachineDescriptor *md) override;
};

class Push_RTL_Stmt : public RTL_Stmt {
  private:
    unsigned int size;

  public:
    Push_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> res, unsigned int sz);
    void print(std::ostream &) override;
    void generateSPIM(SPIM &, MachineDescriptor *md) override;
};

class Pop_RTL_Stmt : public RTL_Stmt {
  private:
    unsigned int size;

  public:
    Pop_RTL_Stmt(unsigned int sz);
    void print(std::ostream &) override;
    void generateSPIM(SPIM &, MachineDescriptor *md) override;
};

class RTL {
    std::vector<std::shared_ptr<RTL_Stmt>> rtl_code;
    static std::map<std::string, unsigned int> string_to_int;

  public:
    MachineDescriptor *machine_descriptor;
    RTL();
    ~RTL();

    static void set_string_to_int_map(std::map<std::string, unsigned int>);
    void print(std::ostream &);
    unsigned int getStringConstNum(std::string s);
    void addRTLStatement(std::shared_ptr<RTL_Stmt> stmt);
    bool isEmpty();
    void generateSPIM(SPIM &spim);
};
