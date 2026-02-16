#pragma once
#include "types.hpp"
#include <string>
#include <map>

class SymbolTableEntry{
	std::string name;
	DataType type;
	int lineno;
public:
	SymbolTableEntry(std::string n, DataType dt);
	DataType get_type() const;
};

class SymbolTable{
	std::map<std::string, SymbolTableEntry*> entries;
    SymbolTable* parent;
public:
	SymbolTable(SymbolTable * p = nullptr);
	~SymbolTable();

	void insert(std::string name, DataType dt);
	SymbolTableEntry* lookup(std::string name);
};
