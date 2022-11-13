#include "InstructionSet.h"

struct InstructionExcept : public std::exception
{
	int lineNumber;
	
	InstructionExcept(const std::string& error, int n)
		: std::exception(error.c_str()), lineNumber(n)
	{
	}
};


inline std::ostream& operator<<(std::ostream& out, const InstructionExcept& error)
{
	out << "Expected " << error.what() << " at line " << error.lineNumber;
	return out;
}

void InstructionSet::AddInstruction(std::string const& m, int opcode, Instruction::OperandType t)
{
	_instSet.push_back(Instruction(m, opcode, t));
}

InstructionSet::SetIterator InstructionSet::FindInstruction(std::string const& m)
{
	SetIterator it;
	for (it = _instSet.begin(); it != _instSet.end(); it++)
	{
		if ((*it).mnemonic == m)
			break;
	}
	return it;
}

InstructionSet::SetConstIterator InstructionSet::FindInstruction(std::string const& m) const
{
	SetConstIterator it;
	for (it = _instSet.begin(); it != _instSet.end(); it++)
	{
		if ((*it).mnemonic == m)
			break;
	}
	return it;
}

bool InstructionSet::Exists(std::string const& m) const
{
	return FindInstruction(m) != _instSet.end();
}

Instruction& InstructionSet::operator[](std::string const& m)
{
	SetIterator it = FindInstruction(m);
	return (*it);
}

Instruction const& InstructionSet::operator[](std::string const& m) const
{
	SetConstIterator it = FindInstruction(m);
	return (*it);
}

bool InstructionSet::BuildSet(Tokenizer& t)
{
	try
	{
		for (;;)
		{
			Token token = t.GetToken();
			if (token.Type() != Token::TokenType::STRING)
				throw InstructionExcept("string", t.LineNumber());
			std::string mnemonic = token.Value();
			t.NextToken();

			token = t.GetToken();
			if (token.Type() != Token::TokenType::COMMA)
				throw InstructionExcept("comma", t.LineNumber());
			t.NextToken();
			token = t.GetToken();
			if (token.Type() != Token::TokenType::NUMBER)
				throw InstructionExcept("number", t.LineNumber());
			int opcode = token.NumValue();
			t.NextToken();

			token = t.GetToken();
			if (token.Type() != Token::TokenType::COMMA)
				throw InstructionExcept("comma", t.LineNumber());
			t.NextToken();
			token = t.GetToken();
			if (token.Type() != Token::TokenType::STRING)
				throw InstructionExcept("operand type", t.LineNumber());
			std::string opType = token.Value();

			Instruction::OperandType type;
			switch (opType.at(0))
			{
			case 'B':
				type = Instruction::OperandType::BYTE;
				break;
			case 'W':
				type = Instruction::OperandType::WORD;
				break;
			case 'N':
				type = Instruction::OperandType::NONE;
				break;
			default:
				throw InstructionExcept("operand type", t.LineNumber());
			}

			AddInstruction(mnemonic, opcode, type);

			t.NextToken();
			token = t.GetToken();
			if (token.Type() != Token::TokenType::EOL)
				throw InstructionExcept("end of line", t.LineNumber());

			while (token.Type() == Token::TokenType::EOL)
			{
				t.NextToken();
				token = t.GetToken();
			}
			if (token.Type() == Token::TokenType::END)
				return true;
		}
	}
	catch (InstructionExcept& e)
	{
		std::cout << "FATAL ERROR IN INSTRUCTION SET:  ";
		std::cout << e << '\n';
		return false;
	}
}

void InstructionSet::Show() const
{
	SetConstIterator it;

	for (it = _instSet.begin(); it != _instSet.end(); it++)
		std::cout << *it << '\n';
}

std::ostream& operator<<(std::ostream& out, Instruction const& inst)
{
	out << inst.mnemonic << "(" << std::hex << inst.opcodeBase << ")";
	switch (inst.type)
	{
	case Instruction::OperandType::NONE:
		out << "N";
		break;
	case Instruction::OperandType::BYTE:
		out << "B";
		break;
	case Instruction::OperandType::WORD:
		out << "W";
		break;
	}
	return out;
}

void InstructionSet::GenerateOpcodes(std::ostream& os) const
{
	SetConstIterator it;

	// Output the VHDL preamble
	os << "library ieee;" << std::endl;
	os << "use ieee.std_logic_1164.all;" << std::endl;
	os << "PACKAGE opcodes IS" << std::endl;


	for (it = _instSet.begin(); it != _instSet.end(); it++)
		os << "    CONSTANT " << it->mnemonic << " = " << ((it->opcodeBase) >> 8) << ";" << std::endl;

	os << "END PACKAGE;" << std::endl;
}

