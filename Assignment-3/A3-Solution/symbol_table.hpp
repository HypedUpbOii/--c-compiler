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
	std::map<std::string, std::pair<DataType, std::vector<DataType>>> procedures;
    SymbolTable* parent;
	bool encounteredDuplicate;
public:
	SymbolTable(SymbolTable * p = nullptr);
	~SymbolTable();

	void insert(std::string name, DataType dt);
	void insertProcedure(std::string name, std::pair<DataType, std::vector<DataType>> signature);
	SymbolTableEntry* lookup(std::string name);
	bool hasDuplicate();
};
