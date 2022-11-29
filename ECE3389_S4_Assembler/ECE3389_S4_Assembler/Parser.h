#ifndef INC_PARSER_H
#define INC_PARSER_H

#include <exception>
#include <iostream>
#include <vector>

#include "Tokenizer.h"
#include "State.h"
#include "Memory.h"
#include "SymbolTable.h"
#include "InstructionSet.h"

struct Branch
{
	std::string destLabel;
	unsigned int sourceLocation;

	Branch(std::string const& d, unsigned int s)
		: destLabel(d), sourceLocation(s)
	{
	}

	Branch& operator=(Branch const& rhs)
	{
		destLabel = rhs.destLabel;
		sourceLocation = rhs.sourceLocation;
		return *this;
	}
};

struct Pointer
{
	std::string name;
	unsigned int memoryLocation;

	Pointer(std::string const& n, unsigned int loc)
		: name(n),
		memoryLocation(loc)
	{
	}
};


class Parser;
typedef State<Parser> ParserState;

class Parser
{

public:
	Parser(Tokenizer& tokenizer, InstructionSet& set);
	void Parse();
	void GenerateObjectFile(std::ostream&);
	
	struct ParserError : public std::exception
	{
		int lineNumber;

		ParserError(std::string const& error, int lineNumber)
			: std::exception(error.c_str()), lineNumber(lineNumber)
		{
		}
	};

private:
	void FatalError(const char* error);
	void FatalError(const char* error, std::exception& e);
	void NonFatalError(const char* error, std::exception& e);
	void NonFatalError(const char* error, bool endLine = true);
	void AdvanceTokenizer();
	void IgnoreComments();


private:

	ParserState FileState();
	ParserState EatLine();
	ParserState OriginState();
	ParserState DataWordState();
	ParserState InstructionState();
	ParserState EndOfState();
	ParserState LabelHandlerState();
	ParserState BranchOffsetState();

	ParserState ProcessByteOperand();
	ParserState ProcessWordOperand();

	bool ValidMnemonic(std::string const&);
	void ResolveAllBranches();
	void ResolveAllPointers();


private:
	Tokenizer& _tokenizer;
	InstructionSet& _instSet;
	Token _token;
	Memory _memory;
	std::vector<Branch> _branches;
	std::vector<Pointer> _pointers;
	SymbolTable _table;
	ParserState _currentState;
};

inline std::ostream& operator<<(std::ostream& out, const Parser::ParserError& e)
{
	out << e.what() << e.lineNumber;
	return out;
}

inline ParserState Parser::EndOfState(void)
{
	return &Parser::EndOfState;
}
#endif
