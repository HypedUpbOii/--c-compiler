#include "symbol_table.hpp"
#include <iostream>

SymbolTableEntry::SymbolTableEntry(std::string n, DataType dt) : name(n), type(dt) {}

DataType SymbolTableEntry::get_type() const { return type; }

std::string SymbolTableEntry::get_name() const { return name; }

SymbolTable::SymbolTable(SymbolTable * p) : parent(p), encounteredDuplicate(false) {}

SymbolTable::~SymbolTable() {
	// delete all STEs
	for (auto & [_, ptr] : entries) {
		delete ptr;
	}
	entries.clear();
}

// insertion always happens in the current scope
void SymbolTable::insert(std::string name, DataType dt) {
	if (procedures.find(name) != procedures.end()) {
		encounteredDuplicate = true;
	}

	if (entries.find(name) != entries.end()) {
		encounteredDuplicate = true;
	}

	entries[name] = new SymbolTableEntry(name, dt);
}


void SymbolTable::insertProcedure(std::string name, std::pair<DataType, std::vector<DataType>> signature) {
	// first check if any variable has the same name
	if (entries.find(name) != entries.end()) {
		encounteredDuplicate = true;
	}

	// then check if any function has the same name (No overloading present)
	if (procedures.find(name) != procedures.end()) {
		encounteredDuplicate = true;
	}

	procedures[name] = signature;
}

// recursively look up in ancestor scopes
SymbolTableEntry * SymbolTable::lookup(std::string name) {
    if (entries.find(name) != entries.end()) return entries[name];

    if (parent != nullptr) return parent->lookup(name);

	return nullptr;
}

bool SymbolTable::hasDuplicate() {
	return encounteredDuplicate;
}
