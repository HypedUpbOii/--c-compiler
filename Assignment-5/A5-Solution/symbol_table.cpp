#include "symbol_table.hpp"
#include <iostream>

SymbolTableEntry::SymbolTableEntry(std::string n, DataType dt, int stack_pos)
    : name(n), type(dt), stack_position(stack_pos) {}

DataType SymbolTableEntry::get_type() const { return type; }

std::string SymbolTableEntry::get_name() const { return name; }

bool SymbolTableEntry::get_need_float() const {
    return type.base == BaseType::FLOAT;
}

SymbolTableFunction::SymbolTableFunction(std::string n, DataType rt,
                                         std::vector<DataType> &params,
                                         bool def)
    : name(n), return_type(rt), sub_signature(params), is_defined(def) {}

DataType SymbolTableFunction::get_return_type() const { return return_type; }

std::string SymbolTableFunction::get_name() const { return name; }

const std::vector<DataType> &SymbolTableFunction::get_params() const {
    return sub_signature;
}

SymbolTable::SymbolTable(SymbolTable *p, DataType dt)
    : parent(p), encounteredDuplicate(false), stackLocals(0), stackParams(8) {
    if (dt == BaseType::VOID) {
        return_stemp = nullptr;
    } else {
        insert_stemp("stemp0", dt);
        return_stemp = stemps["stemp0"];
    }
}

SymbolTable::~SymbolTable() {
    // delete all STEs
    for (auto &[_, ptr] : entries)
        delete ptr;

    for (auto &[_, ptr] : stemps)
        delete ptr;

    for (auto &[_, ptr] : funcs)
        delete ptr;
    entries.clear();
}

// insertion always happens in the current scope
void SymbolTable::insert(std::string name, DataType dt, bool is_param) {
    name = (name == "main") ? "main" : name + "_";
    if (func_lookup(name)) {
        encounteredDuplicate = true;
        return;
    }
    if (entries.find(name) != entries.end()) {
        encounteredDuplicate = true;
        return;
    }

    int stack_pos = 0;
    if (is_param) {
        stack_pos = stackParams;
        if (dt.base == BaseType::FLOAT)
            stackParams += 8;
        else
            stackParams += 4;
    } else {
        if (dt.base == BaseType::FLOAT)
            stackLocals -= 8;
        else
            stackLocals -= 4;
        stack_pos = stackLocals;
    }
    entries[name] = new SymbolTableEntry(name, dt, stack_pos);
}

void SymbolTable::insert_stemp(std::string name, DataType dt) {
    if (stemps.find(name) != stemps.end())
        exit_with_err_msg("BRUH");

    if (dt.base == BaseType::FLOAT)
        stackLocals -= 8;
    else
        stackLocals -= 4;
    stemps[name] = new SymbolTableEntry(name, dt, stackLocals);
}

void SymbolTable::insert_func(std::string name, DataType rt,
                              std::vector<DataType> params, bool def) {
    name = (name == "main") ? "main" : name + "_";
    if (lookup(name) != nullptr) {
        encounteredDuplicate = true;
        return;
    }
    if (func_lookup(name) != nullptr) {
        if (funcs[name]->get_return_type() != rt ||
            funcs[name]->get_params() != params) {
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

SymbolTableEntry *SymbolTable::stemp_lookup(std::string name) {
    if (stemps.find(name) != stemps.end())
        return stemps[name];

    if (parent != nullptr)
        return parent->stemp_lookup(name);

    return nullptr;
}

SymbolTableFunction *SymbolTable::func_lookup(std::string name) {
    if (funcs.find(name) != funcs.end())
        return funcs[name];

    if (parent != nullptr)
        return parent->func_lookup(name);

    return nullptr;
}

int SymbolTable::getStackSpace() { return stackLocals; }

bool SymbolTable::hasDuplicate() { return encounteredDuplicate; }
