#pragma once
#include "common_utils.hpp"
#include <map>
#include <string>
#include <vector>

class SymbolTableEntry {
    std::string name;
    DataType type;

  public:
    SymbolTableEntry(std::string n, DataType dt);
    DataType get_type() const;
    std::string get_name() const;
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
    std::map<std::string, SymbolTableFunction *> funcs;
    SymbolTable *parent;
    bool encounteredDuplicate;

  public:
    SymbolTable(SymbolTable *p = nullptr);
    ~SymbolTable();

    void insert(std::string name, DataType dt);
    void insert_func(std::string, DataType, std::vector<DataType>,
                     bool = false);
    SymbolTableEntry *lookup(std::string name);
    SymbolTableFunction *func_lookup(std::string name);
    bool hasDuplicate();
};
