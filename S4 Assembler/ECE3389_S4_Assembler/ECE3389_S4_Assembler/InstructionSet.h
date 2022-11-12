#ifndef INC_INSTRUCTIONSET_H
#define INC_INSTRUCTIONSET_H

#include <vector>
#include <string>
#include <iostream>
#include "Tokenizer.h"

struct Instruction
{
	enum class OperandType
	{
		NONE,
		BYTE,
		WORD
	};

	std::string mnemonic;
	unsigned int opcodeBase;
	OperandType type;

	Instruction(std::string const& m, int opcode, OperandType t)
		: mnemonic(m),
		opcodeBase(opcode << 8),
		type(t)
	{
	}

	Instruction& operator=(Instruction const& rhs)
	{
		mnemonic = rhs.mnemonic;
		opcodeBase = rhs.opcodeBase;
		type = rhs.type;
		return (*this);
	}
};

std::ostream& operator<<(std::ostream& out, Instruction const& inst);

class InstructionSet
{
public:
	InstructionSet()
	{
	}

	void AddInstruction(std::string const& m, int opcode, Instruction::OperandType t);
	bool Exists(std::string const& m) const;
	Instruction const& operator[](std::string const& m) const;
	Instruction& operator[](std::string const& m);
	bool BuildSet(Tokenizer& t);
	void Show() const;
	void GenerateOpcodes(std::ostream& os) const;
	
private:
	std::vector<Instruction> _instSet;
	typedef std::vector<Instruction>::iterator SetIterator;
	typedef std::vector<Instruction>::const_iterator SetConstIterator;

private:
	SetIterator FindInstruction(std::string const& label);
	SetConstIterator FindInstruction(std::string const& label) const;
};

#endif
