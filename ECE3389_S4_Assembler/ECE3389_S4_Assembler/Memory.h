#ifndef INC_MEMORY_H
#define INC_MEMORY_H

#include <exception>
#include <string>
#include <vector>

struct MemoryError : public std::exception
{
	MemoryError(const std::string& error)
		: std::exception(error.c_str())
	{
	}
};

class Memory
{

public:
	Memory(unsigned int size)
		: _dataBlock(size, 0), _memoryPointer(0)
	{
	}

	void SetMemoryPointer(unsigned int newValue)
	{
		if (_memoryPointer > _dataBlock.size())
			throw MemoryError(std::string("Address out of range"));
		_memoryPointer = newValue;
	}

	void Write(int newValue)
	{
		_dataBlock[_memoryPointer] = newValue;
	}

	void WriteLowByte(char newValue)
	{
		int temp = _dataBlock[_memoryPointer];
		temp &= 0xFF00;
		temp |= (newValue & 0xFF);
		_dataBlock[_memoryPointer] = temp;
	}

	void WriteHighByte(char newValue)
	{
		_dataBlock[_memoryPointer] &= 0x00FF;
		_dataBlock[_memoryPointer] |= (((int)newValue) << 8);
	}

	void Advance()
	{
		++_memoryPointer;
	}

	int Read() const
	{
		return _dataBlock[_memoryPointer];
	}
	unsigned int CurrentLoc() const
	{
		return _memoryPointer;
	}

private:
	std::vector<int> _dataBlock;
	unsigned int _memoryPointer;
};
#endif

