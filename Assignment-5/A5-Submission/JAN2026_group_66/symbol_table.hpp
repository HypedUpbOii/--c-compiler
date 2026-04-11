#pragma once
#include "common_utils.hpp"
#include <map>

class SymbolTableEntry {
    std::string name;
    DataType type;

  public:
    bool is_global;
    int stack_position;
    SymbolTableEntry(std::string n, DataType dt, int stack_pos);
    DataType get_type() const;
    std::string get_name() const;
    bool get_need_float() const;
};

class SymbolTableFunction {
    std::string name;
    DataType return_type;
    std::vector<DataType> sub_signature;

  public:
    bool is_defined;
    SymbolTableFunction(std::string, DataType, std::vector<DataType> &,
                        bool = false);
    DataType get_return_type() const;
    std::string get_name() const;
    const std::vector<DataType> &get_params() const;
};

class SymbolTable {
    std::map<std::string, SymbolTableEntry *> entries;
    std::map<std::string, SymbolTableEntry *> stemps;
    std::map<std::string, SymbolTableFunction *> funcs;
    SymbolTable *parent;
    bool encounteredDuplicate;
    int stackLocals;
    int stackParams;

  public:
    SymbolTableEntry *return_stemp;
    SymbolTable(SymbolTable *p = nullptr, DataType dt = BaseType::VOID);
    ~SymbolTable();

    void insert(std::string name, DataType dt, bool is_param = false);
    void insert_stemp(std::string name, DataType dt);
    void insert_func(std::string, DataType, std::vector<DataType>,
                     bool = false);
    void setReturnStemp(DataType dt);
    SymbolTableEntry *lookup(std::string name);
    SymbolTableEntry *stemp_lookup(std::string name);
    SymbolTableFunction *func_lookup(std::string name);
    int getStackSpace();
    bool hasDuplicate();
    void mark_global(std::string);
};
