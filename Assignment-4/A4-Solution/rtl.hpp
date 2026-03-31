#pragma once
#include "common_utils.hpp"
#include "symbol_table.hpp"
#include "register.hpp"
#include <string>
#include <vector>
#include <map>

class RTL_Opd {
public:
    OpdType opd_type;
    virtual std::string get_name() = 0;
};

class RTL_Double_Const_Opd : public RTL_Opd {
    double value;
public:
    RTL_Double_Const_Opd(double v);
    std::string get_name() override;
};

class RTL_Int_Const_Opd : public RTL_Opd {
    int value;
public:
    RTL_Int_Const_Opd(int v);
    std::string get_name() override;
};

class RTL_Label_Opd : public RTL_Opd {
    unsigned int label_num;
public:
    RTL_Label_Opd(int num);
    std::string get_name() override;
};

class RTL_Register_Opd : public RTL_Opd {
    public:
    RegisterDescriptor * reg_desc;
    RTL_Register_Opd(RegisterDescriptor * rd);
    std::string get_name() override;
};

class RTL_String_Const_Opd : public RTL_Opd {
    unsigned int string_num;
    std::string value;
public:
    RTL_String_Const_Opd(unsigned int str_num, std::string s);
    std::string get_name() override;
    unsigned int get_string_num() const;
};

class RTL_Var_Opd : public RTL_Opd {
public:
    SymbolTableEntry * entry;
    RTL_Var_Opd(SymbolTableEntry * e);
    std::string get_name() override;
};

// ---------------------------------------------------------------

class RTL_Stmt {
protected:
    RTL_Opd * result;
    RTL_Opd * oper1;
    RTL_Opd * oper2;
public:
    virtual void print(std::ostream &) = 0;
};


class Compute_RTL_Stmt : public RTL_Stmt {
protected:
    bool isfloat;
};

class Arithmetic_RTL_Stmt : public Compute_RTL_Stmt {
    ArithmeticOperator oper;
public:
    Arithmetic_RTL_Stmt(RTL_Register_Opd * res, RTL_Register_Opd * op1, RTL_Register_Opd * op2, ArithmeticOperator opr);
    void print(std::ostream & out) override;
};

class Boolean_RTL_Stmt : public Compute_RTL_Stmt {
    BooleanOperator oper;
public:
    Boolean_RTL_Stmt(RTL_Register_Opd * res, RTL_Register_Opd * op1, RTL_Register_Opd * op2, BooleanOperator opr);
    void print(std::ostream & out) override;
};

class Relational_RTL_Stmt : public Compute_RTL_Stmt {
    RelationalOperator oper;

public:
    Relational_RTL_Stmt(RTL_Register_Opd * res, RTL_Register_Opd * op1, RTL_Register_Opd * op2, RelationalOperator opr);
    Relational_RTL_Stmt(RTL_Register_Opd * op1, RTL_Register_Opd * op2, RelationalOperator opr);
    void print(std::ostream & out) override;
};

class UMinus_RTL_Stmt : public Compute_RTL_Stmt {
public:
    UMinus_RTL_Stmt(RTL_Register_Opd * res, RTL_Register_Opd * op);
    void print(std::ostream & out) override;
};

class Not_RTL_Stmt : public Compute_RTL_Stmt {
public:
    Not_RTL_Stmt(RTL_Register_Opd * res, RTL_Register_Opd * op);
    void print(std::ostream & out) override;
};


class Control_Flow_RTL_Stmt : public RTL_Stmt {

};

class Call_RTL_Stmt : public Control_Flow_RTL_Stmt {
    // TODO L5
};

class Goto_RTL_Stmt : public Control_Flow_RTL_Stmt {
public:
    Goto_RTL_Stmt(RTL_Label_Opd * l);
    void print(std::ostream &) override;
};

class If_Goto_RTL_Stmt : public Control_Flow_RTL_Stmt {
public:
    // register -> oper1, label -> result
    If_Goto_RTL_Stmt(RTL_Register_Opd * r, RTL_Label_Opd * l);
    void print(std::ostream &) override;
};

class Return_RTL_Stmt : public Control_Flow_RTL_Stmt {
    // TODO L5
};


class Label_RTL_Stmt : public RTL_Stmt {
public:
    Label_RTL_Stmt(RTL_Label_Opd * l);
    void print(std::ostream &) override;
};


class Transfer_RTL_Stmt : public RTL_Stmt {
    OpdType dest_type;
    OpdType src_type;
    bool isfloat;
public:
    Transfer_RTL_Stmt(RTL_Register_Opd * dest, RTL_Register_Opd * src);
    Transfer_RTL_Stmt(RTL_Var_Opd * dest, RTL_Register_Opd * src);
    Transfer_RTL_Stmt(RTL_Register_Opd * dest, RTL_Var_Opd * src);
    Transfer_RTL_Stmt(RTL_Register_Opd * dest, RTL_Int_Const_Opd * src);
    Transfer_RTL_Stmt(RTL_Register_Opd * dest, RTL_Double_Const_Opd * src);
    Transfer_RTL_Stmt(RTL_Register_Opd * dest, RTL_String_Const_Opd * src);

    void print(std::ostream &) override;
};


class NOP_RTL_Stmt : public RTL_Stmt {

};


class Read_RTL_Stmt : public RTL_Stmt {
public:
    Read_RTL_Stmt();
    void print(std::ostream &) override;
};


class Write_RTL_Stmt : public RTL_Stmt {
public:
    Write_RTL_Stmt();
    void print(std::ostream &) override;
};

class Mov_RTL_Stmt : public RTL_Stmt {
    unsigned int flag;
    bool movt;
public:
    Mov_RTL_Stmt(RTL_Register_Opd * res, RTL_Register_Opd * opd, unsigned int _flag, bool _movt);
    void print(std::ostream &) override;
};

class RTL {
    std::vector<RTL_Stmt *> rtl_code;
    unsigned int string_const_num;
    std::map<std::string, unsigned int> string_to_int;
    
public:
    MachineDescriptor * machine_descriptor;
    RTL();
    ~RTL();

    void print(std::ostream &);
    void addNewStringConst(std::string s); // add only if it doesn't exist
    unsigned int getStringConstNum(std::string s);
    void addRTLStatement(RTL_Stmt * stmt);
};