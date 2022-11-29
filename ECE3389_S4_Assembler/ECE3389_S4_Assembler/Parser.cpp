#include <iomanip>
#include <algorithm>

#include "Parser.h"

static constexpr int MEMORY_SIZE = 512;

Parser::Parser(Tokenizer& tokenizer, InstructionSet& set)
	: _tokenizer(tokenizer), _instSet(set),	_token(_tokenizer.GetToken()),
	_memory(MEMORY_SIZE), _currentState(&Parser::FileState, this)
{
}

void Parser::Parse()
{
	while (_currentState != &Parser::EndOfState)
		_currentState = _currentState();
}

inline void Parser::FatalError(const char* error)
{
	throw Parser::ParserError(std::string(error) + " in line ", _tokenizer.LineNumber());
}

inline void Parser::FatalError(const char* error, std::exception& e)
{
	std::cout << "ERROR: " << e.what() << "." << std::endl;
	FatalError(error);
}

inline void Parser::NonFatalError(const char* error, bool bEndLine/*=true*/)
{
	std::cout << "ERROR: " << error << " in line " << _tokenizer.LineNumber();
	if (bEndLine)
		std::cout << "." << std::endl;
}

inline void Parser::NonFatalError(const char* error, std::exception& e)
{
	NonFatalError(error, false);
	std::cout << ".  " << e.what() << "." << std::endl;
}

inline void Parser::AdvanceTokenizer()
{
	try
	{
		_tokenizer.NextToken();
	}
	catch (Tokenizer::TokenizerError& e)
	{
		FatalError("Invalid token", e);
	}
	_token = _tokenizer.GetToken();
}

inline void Parser::IgnoreComments()
{
	while (_token.Type() == Token::TokenType::COMMENT)
	{
		AdvanceTokenizer();
	}
}

inline bool Equiv(const std::string& lhs, const char* rhs)
{
	std::string s1(lhs);
	std::string s2(rhs);
	std::transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
	std::transform(s2.begin(), s2.end(), s2.begin(), ::toupper);
	return s1 == s2;
}

ParserState Parser::EatLine()
{
	while (_token.Type() != Token::TokenType::EOL)
		AdvanceTokenizer();
	AdvanceTokenizer();
	return &Parser::FileState;
}

ParserState Parser::FileState()
{
	while (true)
	{
		if ((_token.Type() == Token::TokenType::COMMENT) ||
			(_token.Type() == Token::TokenType::EOL))
		{
			IgnoreComments();
			EatLine();
		}
		else if (_token.Type() == Token::TokenType::STRING)
		{
			if (Equiv(_token.Value(), "ORG"))
			{
				AdvanceTokenizer();
				return &Parser::OriginState;
			}
			else if (Equiv(_token.Value(), "DCW"))
			{
				AdvanceTokenizer();
				return &Parser::DataWordState;
			}
			else if (ValidMnemonic(_token.Value()))
				return &Parser::InstructionState;
			else
			{
				return &Parser::LabelHandlerState;
			}
		}
		else if (_token.Type() == Token::TokenType::END)
		{
			ResolveAllBranches();
			ResolveAllPointers();
			return &Parser::EndOfState;
		}
		else
		{
			NonFatalError("Expected mnemonic or directive but found unexpected token");
			return &Parser::EatLine;
		}
	}

	return &Parser::FileState;
}

void Parser::ResolveAllBranches()
{
	for (unsigned int br = 0; br < _branches.size(); br++)
	{
		Branch branch = _branches[br];

		if (_table.Resolved(branch.destLabel))
		{
			unsigned int offset = _table[branch.destLabel] - (branch.sourceLocation + 1);
			_memory.SetMemoryPointer(branch.sourceLocation);
			_memory.WriteLowByte((char)offset);
		}
		else
		{
			NonFatalError("There were unresolved labels");
		}
	}
}

void Parser::ResolveAllPointers()
{
	for (unsigned int p = 0; p < _pointers.size(); p++)
	{
		Pointer ptr = _pointers[p];

		if (_table.Resolved(ptr.name))
		{
			_memory.SetMemoryPointer(ptr.memoryLocation);
			_memory.Write(_table[ptr.name]);
		}
		else
		{
			NonFatalError("There were unresolved labels");
		}
	}
}

ParserState Parser::LabelHandlerState()
{
	std::string thisLabel = _token.Value();
	AdvanceTokenizer();

	if (_token.Type() == Token::TokenType::STRING)
	{
		if (ValidMnemonic(_token.Value()))
		{
			_table.AddLabel(thisLabel, _memory.CurrentLoc());
			return &Parser::InstructionState;
		}
		else if (Equiv(_token.Value(), "DCW"))
		{
			_table.AddLabel(thisLabel, _memory.CurrentLoc());
			AdvanceTokenizer();
			return &Parser::DataWordState;
		}
		else if (Equiv(_token.Value(), "EQU"))
		{
			AdvanceTokenizer();
			if (_token.Type() != Token::TokenType::NUMBER)
			{
				NonFatalError("Expected numerical data");
			}
			else
			{
				_table.AddLabel(thisLabel, _token.NumValue());
			}
			return &Parser::EatLine;
		}
	}
	NonFatalError("Either an instruction, a DCW or an EQU must follow label");
	return &Parser::EatLine;
}

ParserState Parser::BranchOffsetState(void)
{
	try
	{
		if (_token.Type() != Token::TokenType::OPEN)
			throw "open parentheses";
		AdvanceTokenizer();
		if (_token.Type() != Token::TokenType::STRING)
			throw "destination label";
		std::string target = _token.Value();
		AdvanceTokenizer();
		if (_token.Type() != Token::TokenType::CLOSE)
			throw "close parentheses";

		if (_table.Resolved(target))
		{
			int offset = int(_table[target]) - int(_memory.CurrentLoc() + 1);
			_memory.WriteLowByte((char)offset);
			_memory.Advance();
		}
		else
		{
			_memory.WriteLowByte(0);
			_branches.push_back(Branch(target, _memory.CurrentLoc()));
			_memory.Advance();
		}
		return &Parser::EatLine;
	}
	catch (char const* str)
	{
		std::ostringstream os;
		os << "Expected " << std::string(str) << " in branch offset";
		NonFatalError(os.str().c_str());
		return &Parser::EatLine;
	}
}

ParserState Parser::OriginState(void)
{
	if (_token.Type() != Token::TokenType::NUMBER)
	{
		NonFatalError("Expected numerical address");
		AdvanceTokenizer();
		return &Parser::EatLine;
	}
	else
	{
		_memory.SetMemoryPointer(_token.NumValue());
		AdvanceTokenizer();
		return &Parser::EatLine;
	}
}


ParserState Parser::DataWordState(void)
{
	while (true)
	{
		if (_token.Type() != Token::TokenType::NUMBER)
		{
			NonFatalError("Expected numerical data");
			return &Parser::EatLine;
		}
		else
		{
			_memory.Write(_token.NumValue());
			AdvanceTokenizer();
			_memory.Advance();
			if (_token.Type() == Token::TokenType::COMMA)
				AdvanceTokenizer();
			else
				return &Parser::EatLine;
		}
	}

}

bool Parser::ValidMnemonic(std::string const& mnemonic)
{
	return _instSet.Exists(mnemonic);
}

ParserState Parser::InstructionState(void)
{
	std::string mnemonic(_token.Value());
	AdvanceTokenizer();

	// If we got here, we know this is a valid mnemonic
	Instruction inst = _instSet[mnemonic];
	_memory.Write(inst.opcodeBase);
	switch (inst.type)
	{
	case Instruction::OperandType::BYTE:
		return ProcessByteOperand();
	case Instruction::OperandType::WORD:
		return ProcessWordOperand();
	case Instruction::OperandType::NONE:
		_memory.Advance();
		return &Parser::EatLine;

	}

	//If we got here, ain't no such mnemonic
	NonFatalError("Invalid mnemonic");
	AdvanceTokenizer();
	return &Parser::EatLine;
}

ParserState Parser::ProcessByteOperand(void)
{
	if (_token.Type() == Token::TokenType::STRING)
	{
		if (_table.Resolved(_token.Value()))
		{
			int operand = int(_table[_token.Value()]);
			if ((operand > 127) || (operand < -128))
				NonFatalError("Byte operand out of range.  It will be truncated.");
			_memory.WriteLowByte((char)operand);
			_memory.Advance();
		}
		else
			NonFatalError("Unresolved labels cannot be used as byte operands.");

		return &Parser::EatLine;
	}
	else if (_token.Type() == Token::TokenType::NUMBER)
	{
		int operand = _token.NumValue();
		if ((operand > 127) || (operand < -128))
			NonFatalError("Byte operand out of range.  It will be truncated.");
		_memory.WriteLowByte((char)operand);
		_memory.Advance();
		return &Parser::EatLine;
	}
	else if (_token.Type() == Token::TokenType::BROFFS)
	{
		AdvanceTokenizer();
		return &Parser::BranchOffsetState;
	}
	else
		NonFatalError("Expected byte operand.");

	return &Parser::EatLine;
}

//BUGBUG: check signed/unsigned ness of memory class, memory content class, branch class, label class.
// My gut is telling me the only thing that can be signed is a branch offset, but maybe an operand can be?

ParserState Parser::ProcessWordOperand(void)
{
	_memory.Advance();
	if (_token.Type() == Token::TokenType::STRING)
	{
		if (_table.Resolved(_token.Value()))
		{
			_memory.Write(_table[_token.Value()]);
			_memory.Advance();
		}
		else
		{
			_pointers.push_back(Pointer(_token.Value(), _memory.CurrentLoc()));
			_memory.Advance();
		}

		return &Parser::EatLine;
	}
	else if (_token.Type() == Token::TokenType::NUMBER)
	{
		_memory.Write(_token.NumValue());
		_memory.Advance();
		return &Parser::EatLine;
	}
	else
		NonFatalError("Expected word operand.");

	return &Parser::EatLine;
}

void Parser::GenerateObjectFile(std::ostream& object)
{
	object << "WIDTH" << '\t' << "=" << '\t' << "16;" << std::endl;
	object << "DEPTH" << '\t' << "=" << '\t' << "512;" << std::endl << std::endl;

	object << "ADDRESS_RADIX" << '\t' << "=" << "\tHEX;" << std::endl;
	object << "DATA_RADIX" << '\t' << "=" << "\tHEX;" << std::endl << std::endl;

	object << "CONTENT" << '\t' << "BEGIN" << std::endl;
	for (unsigned int word = 0; word < MEMORY_SIZE; word++)
	{
		_memory.SetMemoryPointer(word);
		int data = _memory.Read();

		object << std::hex << word << " : " << std::hex << (data & 0xFFFF) << std::endl;
	}
	object << "END;" << std::endl;

}

struct Hex
{
	unsigned int datum;
	int width;
	Hex(int w, const unsigned int& t)
		: datum(t), width(w)
	{
	}
};

inline std::ostream& operator<<(std::ostream& o, const Hex& h)
{
	o.fill('0');
	o << std::setw(h.width) << std::hex << std::uppercase;
	o << h.datum;
	return o;
}
