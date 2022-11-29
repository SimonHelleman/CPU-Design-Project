#include "Tokenizer.h"

bool Tokenizer::IsGrammarCharacter(char c)
{
	switch (c)
	{
	case '$':
	case ';':
	case ',':
	case '#':
	case ')':
	case '(':
		return true;
	default:
		return false;
	}
}

void Tokenizer::NextToken(void)
{
	//We gobble up file input one line at a time

	if ((_stringStream != nullptr) && !*_stringStream)
	{
		// Means we have a valid, but empty string stream
		// so delete it.  This must be the end of the line.
		delete _stringStream;
		_stringStream = nullptr;
		_currentToken._type = Token::TokenType::EOL;
		return;
	}

	if (_stringStream == nullptr)
	{
		//We need to read a new line from the file and
		//create a string stream for it--IF WE ARE NOT
		//AT THE END OF THE FILE!!!
		if (_isStream.eof())
		{
			_currentToken._type = Token::TokenType::END;
			return;
		}
		else
		{
			_lineNumber++;
			_isStream.getline(_inputLine, LINE_LENGTH);
			//We'll check for a bad input stream, and then
			//not bother checking for a bad string stream.
			if (_isStream.bad())
				throw TokenizerError("Unknown error in input file");
			_stringStream = new TokenStringStream(_inputLine);
		}
	}

	//Gobble white space and blank lines at this point
	_stringStream->EatWhite();
	if (_stringStream->EndOfLine())
	{
		delete _stringStream;
		_stringStream = nullptr;

		_currentToken._type = Token::TokenType::EOL;
		return;
	}

	char c = _stringStream->get();

	if (IsGrammarCharacter(c))
	{
		switch (c)
		{
		case ';':
			EatComment();
			break;
		case '$':
			BuildPlainString();
			MakeStringNumber("%X");
			break;
		default:
			HandleGrammarCharacter(c);
			break;
		}
	}
	else if (isdigit(c) || (c == '-'))
	{
		_stringStream->putback(c);
		BuildPlainString();
		MakeStringNumber("%d");
	}
	else
	{
		_stringStream->putback(c);
		BuildPlainString();
	}
}

void Tokenizer::MakeStringNumber(char const* format)
{
	_currentToken._type = Token::TokenType::NUMBER;
	std::string formatString(format);

	std::istringstream is(_currentToken._token);

	if (formatString == "%d")
	{
		is >> _currentToken._number;
		if (!is.eof())
		{
			std::ostringstream os;
			os << "String " << _currentToken._token << " at line " << LineNumber() << " is an invalid decimal format";
			throw TokenizerError(os.str());
		}
	}
	else if (formatString == "%X")
	{
		is >> std::hex >> _currentToken._number;
		if (!is.eof())
		{
			std::ostringstream os;
			os << "String " << _currentToken._token << " at line " << LineNumber() << " is an invalid hex format";
			throw TokenizerError(os.str());
		}
	}
	else
	{
		std::ostringstream os;
		os << "String " << _currentToken._token << " at line " << LineNumber() << " is an invalid unknown format";
		throw TokenizerError(os.str());
	}
}

void Tokenizer::EatComment(void)
{
	_currentToken._type = Token::TokenType::COMMENT;
	while (!_stringStream->EndOfLine())
	{
		_stringStream->get();
	}
}

void Tokenizer::HandleGrammarCharacter(char c)
{
	_currentToken._type = static_cast<Token::TokenType>(c);
}

void Tokenizer::BuildPlainString(void)
{
	_currentToken._token.erase();
	_currentToken._type = Token::TokenType::STRING;
	while (true)
	{
		char c = _stringStream->get();

		if (_stringStream->EndOfLine())
			break;

		//Validate new character
		if (IsGrammarCharacter(c))
		{
			_stringStream->putback(c);
			break;
		}
		if (isspace(c))
			break;

		_currentToken._token += c;
	}
}

bool TokenStringStream::EndOfLine(void)
{
	return !(*this);
}

void TokenStringStream::EatWhite(void)
{
	while (true)
	{
		char c = get();
		if (EndOfLine())
			break;
		if (!isspace(c))
		{
			putback(c);
			break;
		}
	}
}

std::ostream& operator<<(std::ostream& out, Token& token)
{
	switch (token.Type())
	{
	case Token::TokenType::STRING:
		out << "STRING ";
		out << '"' << token.Value() << '"';
		break;
	case Token::TokenType::NUMBER:
		out << "NUMBER " << token.Value();
		break;
	case Token::TokenType::END:
		out << "END OF FILE";
		break;
	case Token::TokenType::COMMENT:
		out << "COMMENT";
		break;
	case Token::TokenType::EOL:
		out << "END OF LINE";
		break;
	default:
		out << static_cast<char>(token.Type()) << " ";
		break;
	}
	return out;
}
