#include "symbol_table.hpp"
#include <iostream>

STE::STE(std::string n, DataType dt) : name(n), type(dt) {}

DataType STE::get_type() const { return type; }

SymbolTable::SymbolTable(SymbolTable * p) : parent(p) {}

SymbolTable::~SymbolTable() {
	// delete all STEs
	for (auto & [_, ptr] : entries) {
		delete ptr;
	}
	entries.clear();
}

// insertion always happens in the current scope
void SymbolTable::insert(std::string name, DataType dt) {
	if (entries.find(name) != entries.end()) {
		std::cerr << "Symbol Table : variable with name " << name << " already declared" << std::endl;
		exit(1);
	}

	entries[name] = new STE(name, dt);
}

// recursively look up in ancestor scopes
STE* SymbolTable::lookup(std::string name) {
    if (entries.find(name) != entries.end()) return entries[name];

    if (parent != nullptr) return parent->lookup(name);

    return nullptr;
}
