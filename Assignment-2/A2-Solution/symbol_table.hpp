#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <map>

enum class DataType{INT_TYPE, FLOAT_TYPE, BOOL_TYPE, VOID_TYPE, STRING_TYPE};

class STE{
	std::string name;
	DataType type;
public:
	STE(std::string n, DataType dt);
	DataType get_type() const;
};

class SymbolTable{
	std::map<std::string, STE*> entries;
    SymbolTable * parent;
public:
	SymbolTable(SymbolTable * p = nullptr);
	// to clean up STEsMvP@2005
	
	~SymbolTable();

	// to insert a new STE
	void insert(std::string name, DataType dt);

	// to search in table
	STE* lookup(std::string name);
};

#endif
