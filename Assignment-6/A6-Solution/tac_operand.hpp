#pragma once
#include "symbol_table.hpp"
#include <memory>

class TAC_Opd {
  protected:
    OpdType opd_type;
    DataType type;

  public:
    virtual std::string get_name() = 0;
    OpdType get_opd_type() const;
    DataType get_data_type() const;
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
  private:
    std::shared_ptr<TAC_Opd> operand;

  public:
    Pointer_Deref_TAC_Opd(std::shared_ptr<TAC_Opd> oper);
    std::string get_name() override;
    std::shared_ptr<TAC_Opd> get_operand();
};

class Address_Of_TAC_Opd : public TAC_Opd {
    SymbolTableEntry *steEntry;

  public:
    Address_Of_TAC_Opd(SymbolTableEntry *ste);
    std::string get_name() override;
    SymbolTableEntry *get_ste();
};

class Temporary_TAC_Opd : public TAC_Opd {
  private:
    unsigned int temp_num;

  public:
    Temporary_TAC_Opd(unsigned int num, DataType type);
    std::string get_name() override;
    unsigned int get_temp_num() const;
};

class Variable_TAC_Opd : public TAC_Opd {
  private:
    SymbolTableEntry *symtab_entry;

  public:
    Variable_TAC_Opd(SymbolTableEntry *symtab_entry);
    std::string get_name() override;
    SymbolTableEntry *get_sym_tab_entry() const;
};

class Function_TAC_Opd : public TAC_Opd {
  private:
    SymbolTableFunction *symtab_entry;
    std::vector<std::shared_ptr<TAC_Opd>> params;

  public:
    Function_TAC_Opd(SymbolTableFunction *symtab_entry,
                     std::vector<std::shared_ptr<TAC_Opd>> args);
    std::string get_name() override;
    SymbolTableFunction *get_sym_tab_func() const;
    std::vector<std::shared_ptr<TAC_Opd>> get_params();
};
