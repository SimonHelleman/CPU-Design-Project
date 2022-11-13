#include "SymbolTable.h"

void SymbolTable::AddLabel(std::string const& label, unsigned int value)
{
	_table.push_back(TableEntry(label, value));
}

SymbolTable::TableIterator SymbolTable::FindLabel(std::string const& label)
{
	TableIterator it;
	for (it = _table.begin(); it != _table.end(); it++)
	{
		if ((*it).label == label)
			break;
	}
	return it;
}

SymbolTable::TableConstIterator SymbolTable::FindLabel(std::string const& label) const
{
	TableConstIterator it;
	for (it = _table.begin(); it != _table.end(); it++)
	{
		if ((*it).label == label)
			break;
	}
	return it;
}

bool SymbolTable::Resolved(std::string const& label) const
{
	return FindLabel(label) != _table.end();
}

unsigned int& SymbolTable::operator[](std::string const& label)
{
	TableIterator it = FindLabel(label);
	return (*it).location;
}

unsigned int SymbolTable::operator[](std::string const& label) const
{
	TableConstIterator it = FindLabel(label);
	return (*it).location;
}
