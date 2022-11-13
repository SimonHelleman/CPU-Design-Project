#ifndef INC_TOKENIZER_H
#define INC_TOKENIZER_H
#include <exception>
#include <iostream>
#include <sstream>

class Tokenizer;

class Token
{
public:
	enum class TokenType
	{
		STRING,
		NUMBER,
		COMMENT,
		EOL,
		END,
		COMMA = ',',
		BROFFS = '#',
		OPEN = '(',
		CLOSE = ')'
	};

public:
	Token()
		: _type(TokenType::STRING), _token(""), _number(0)
	{
	}
	
	TokenType Type() const
	{
		return _type;
	}
	
	const std::string& Value() const
	{
		return _token;
	}

	int NumValue() const
	{
		return _number;
	}
	
	Token& operator=(const Token& rhs)
	{
		_type = rhs._type;
		_token = rhs._token;
		_number = rhs._number;
		return *this;
	}

private:
	TokenType _type;
	std::string _token;
	int _number;
	friend class Tokenizer;
};


class TokenStringStream : public std::istringstream
{
public:
	TokenStringStream(const char* string)
		: std::istringstream(string)
	{
	}
	void EatWhite();
	bool EndOfLine();
};


#define LINE_LENGTH 512

class Tokenizer
{

public:
	//Tokenizer exports only one constructor, Get and Next
	Tokenizer(std::istream& stream)
		: _isStream(stream), _stringStream(0), _lineNumber(0)
	{
		NextToken();
	}
	
	const Token& GetToken() const
	{
		return _currentToken;
	}
	
	void NextToken();

	int LineNumber()
	{
		return _lineNumber;
	}

	struct TokenizerError : public std::exception
	{
		TokenizerError(const std::string& error)
			: std::exception(error.c_str())
		{
		}
	};

private:
	//Member functions used only in the tokenizer
	void EatComment();
	void HandleGrammarCharacter(char c);
	void BuildPlainString();
	bool IsGrammarCharacter(char c);
	void MakeStringNumber(char const* format);

private:
	//Data members are private to the tokenizer
	std::istream& _isStream;   //Use value semantics for the input stream
	TokenStringStream* _stringStream;
	char _inputLine[LINE_LENGTH];
	int _lineNumber;
	Token _currentToken;
};

std::ostream& operator<<(std::ostream& out, Token& token);

#endif
