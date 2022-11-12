#ifndef INC_SYMBOLTABLE_H
#define INC_SYMBOLTABLE_H

#include <vector>
#include <string>

struct TableEntry
{
	std::string label;
	unsigned int location;

	TableEntry(std::string const& lbl, unsigned int loc)
		: label(lbl), location(loc)
	{
	}

	TableEntry& operator=(TableEntry const& rhs)
	{
		label = rhs.label;
		location = rhs.location;

		return *this;
	}
};

class SymbolTable
{
public:
	SymbolTable() {}
	void AddLabel(std::string const& label, unsigned int value);
	bool Resolved(std::string const& label) const;
	unsigned int operator[](std::string const& label) const;
	unsigned int& operator[](std::string const& label);

private:
	std::vector<TableEntry> _table;
	typedef std::vector<TableEntry>::iterator TableIterator;
	typedef std::vector<TableEntry>::const_iterator TableConstIterator;
	
	TableIterator FindLabel(std::string const& label);
	TableConstIterator FindLabel(std::string const& label) const;
};

#endif

