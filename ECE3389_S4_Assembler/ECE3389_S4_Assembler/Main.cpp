#include <iostream>

#include "Tokenizer.h"
#include "Parser.h"
#include "InstructionSet.h"
#include <fstream>
#include <string>

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cout << "USAGE:  S4asm <source> [<instruction set>]\n";
		std::cout << "        A .ASM extension will be assumed for <source> if none is given.\n";
		std::cout << "        If the instruction set is not specified, the file InstructionSet.csv is used.\n";
		return -1;
	}
	else
	{
		std::string sourceFile(argv[1]);
		size_t periodLoc = sourceFile.find('.');
		std::string objectFile;

		if (periodLoc == std::string::npos)
		{
			objectFile = sourceFile + ".MIF";
			sourceFile += ".ASM";
		}
		else
		{
			std::string filebase = sourceFile.substr(0, periodLoc);
			objectFile = filebase + ".MIF";
		}

		std::string instFile("InstructionSet.csv");
		if (argc >= 3)
		{
			instFile = argv[2];
		}

		std::ifstream instStream(instFile, std::ios_base::in);
		if (!instStream.is_open())
		{
			std::cout << "Could not open instruction set file\n";
			return 1;
		}
		std::ifstream inputStream(sourceFile, std::ios_base::in);
		if (!inputStream.is_open())
		{
			std::cout << "Could not open source file\n";
			return 1;
		}
		std::ofstream objectStream(objectFile, std::ios_base::out);
		std::ofstream opcodeStream("opcodes_vhdl.vhdl");

		try 
		{
			Tokenizer instructionizer(instStream);
			InstructionSet instSet;
			
			if (instSet.BuildSet(instructionizer))
			{
				instSet.GenerateOpcodes(opcodeStream);
				Tokenizer tokenizer(inputStream);
				Parser parser(tokenizer, instSet);
				parser.Parse();
				parser.GenerateObjectFile(objectStream);
				std::cout << "Wrote output file " << objectFile << '\n';
				return 1;
			}
			else
			{
				return 0;
			}
		}
		catch (std::exception& e)
		{
			std::cout << e.what() << '\n';
		}
	}
}
