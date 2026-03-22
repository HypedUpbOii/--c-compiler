#pragma once
#include <string>
#include <map>
#include <vector>
#include "common_utils.hpp"

class SymbolTableEntry{
	std::string name;
	DataType type;
	int lineno;
public:
	SymbolTableEntry(std::string n, DataType dt);
	DataType get_type() const;
	std::string get_name() const;
};

class SymbolTable{
	std::map<std::string, SymbolTableEntry*> entries;
    SymbolTable* parent;
	bool encounteredDuplicate;
public:
	SymbolTable(SymbolTable * p = nullptr);
	~SymbolTable();

	void insert(std::string name, DataType dt);
	SymbolTableEntry* lookup(std::string name);
	bool hasDuplicate();
	bool hasFuncVarConflict(const std::vector<std::tuple<DataType, std::string, std::vector<DataType>>>&);
};
