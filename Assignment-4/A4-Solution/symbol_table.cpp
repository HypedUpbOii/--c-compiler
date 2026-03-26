#include "symbol_table.hpp"
#include <iostream>

SymbolTableEntry::SymbolTableEntry(std::string n, DataType dt)
    : name(n), type(dt) {}

DataType SymbolTableEntry::get_type() const { return type; }

std::string SymbolTableEntry::get_name() const { return name; }

SymbolTableFunction::SymbolTableFunction(std::string n, DataType rt,
                                         std::vector<DataType> &params,
                                         bool def)
    : name(n), return_type(rt), sub_signature(params), is_defined(def) {}

DataType SymbolTableFunction::get_return_type() const { return return_type; }

std::string SymbolTableFunction::get_name() const { return name; }

const std::vector<DataType> &SymbolTableFunction::get_params() const {
    return sub_signature;
}

SymbolTable::SymbolTable(SymbolTable *p)
    : parent(p), encounteredDuplicate(false) {}

SymbolTable::~SymbolTable() {
    // delete all STEs
    for (auto &[_, ptr] : entries) {
        delete ptr;
    }
    entries.clear();
}

// insertion always happens in the current scope
void SymbolTable::insert(std::string name, DataType dt) {
    if (func_lookup(name)) {
        encounteredDuplicate = true;
        return;
    }
    if (entries.find(name) != entries.end()) {
        encounteredDuplicate = true;
        return;
    }

    entries[name] = new SymbolTableEntry(name, dt);
}

void SymbolTable::insert_func(std::string name, DataType rt,
                              std::vector<DataType> params, bool def) {
    if (lookup(name) != nullptr) {
        encounteredDuplicate = true;
        return;
    }
    if (func_lookup(name) != nullptr) {
        if (funcs[name]->get_return_type() != rt || funcs[name]->get_params() != params) {
            encounteredDuplicate = true;
            return;
        }
        if (funcs[name]->is_defined) {
            encounteredDuplicate = true;
            return;
        }
        funcs[name]->is_defined = true;
        return;
    }

    funcs[name] = new SymbolTableFunction(name, rt, params, def);
}

// recursively look up in ancestor scopes
SymbolTableEntry *SymbolTable::lookup(std::string name) {
    if (entries.find(name) != entries.end())
        return entries[name];

    if (parent != nullptr)
        return parent->lookup(name);

    return nullptr;
}

SymbolTableFunction *SymbolTable::func_lookup(std::string name) {
    if (funcs.find(name) != funcs.end())
        return funcs[name];

    if (parent != nullptr)
        return parent->func_lookup(name);

    return nullptr;
}

bool SymbolTable::hasDuplicate() { return encounteredDuplicate; }
