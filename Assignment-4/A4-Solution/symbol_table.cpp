#include "symbol_table.hpp"
#include <iostream>

SymbolTableEntry::SymbolTableEntry(std::string n, DataType dt)
    : name(n), type(dt) {}

DataType SymbolTableEntry::get_type() const { return type; }

std::string SymbolTableEntry::get_name() const { return name; }

bool SymbolTableEntry::get_need_float() const { return type.base == BaseType::FLOAT; }

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
    if (entries.find(name) != entries.end()) {
        encounteredDuplicate = true;
        std::cerr << "found variable with name " << name
                  << " which conflicts with variable name" << std::endl;
    }

    entries[name] = new SymbolTableEntry(name, dt);
}

// recursively look up in ancestor scopes
SymbolTableEntry *SymbolTable::lookup(std::string name) {
    if (entries.find(name) != entries.end())
        return entries[name];

    if (parent != nullptr)
        return parent->lookup(name);

    return nullptr;
}

bool SymbolTable::hasDuplicate() { return encounteredDuplicate; }

bool SymbolTable::hasFuncVarConflict(
    const std::vector<std::tuple<DataType, std::string, std::vector<DataType>>>
        &decls) {
    for (auto &[type, name, params] : decls) {
        if (entries.find(name) != entries.end()) {
            return true;
        }
    }

    return false;
}