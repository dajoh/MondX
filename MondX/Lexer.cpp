#include "Lexer.hpp"
#include "OperatorUtil.hpp"

using namespace Mond;

Lexer::Lexer(DiagBuilder &diag, Source &source) :
	m_pos(1, 1),
	m_source(source),
	m_diag(diag)
{
	m_char = m_source.Cur();
	m_peek = m_source.Peek();
}

Token &Lexer::GetToken()
{
	if (IsEof(m_char))
	{
		m_token.type = TokEndOfFile;
		m_token.range = Range(m_pos, 0);
		m_token.slice = Slice(m_source.Position(), m_source.Position());
		return m_token;
	}
	else if (IsWhitespace(m_char))
	{
		return MakeWhitespace();
	}
	else if (m_char == '/' && m_peek == '/')
	{
		return MakeLineComment();
	}
	else if (m_char == '/' && m_peek == '*')
	{
		return MakeBlockComment();
	}
	else if (IsLetter(m_char) || m_char == '_')
	{
		return MakeIdentifier();
	}
	else if (m_char == '\"' || m_char == '\'')
	{
		return MakeStringLiteral();
	}
	else if (IsDecDigit(m_char))
	{
		return MakeNumberLiteral();
	}

	switch (m_char)
	{
	case ':': return MakePunctuation(TokColon);
	case ',': return MakePunctuation(TokComma);
	case ';': return MakePunctuation(TokSemicolon);
	case '(': return MakePunctuation(TokLeftParen);
	case ')': return MakePunctuation(TokRightParen);
	case '{': return MakePunctuation(TokLeftBrace);
	case '}': return MakePunctuation(TokRightBrace);
	case '[': return MakePunctuation(TokLeftBracket);
	case ']': return MakePunctuation(TokRightBracket);
	}

	// TODO: Handle !in and ... properly, and move them out of the operator table.
	// !in isn't handled at all currently.

	if (OperatorLookup1(m_char, m_token.type))
	{
		return MakeOperator();
	}

	m_diag
		<< m_pos
		<< Error
		<< LexUnexpectedCharacter
		<< m_char
		<< DiagEnd;

	m_token.type = TokUnknown;
	m_token.range = Range(m_pos, 1);
	m_token.slice = Slice(m_source.Position());
	Advance();
	return m_token;
}

void Mond::Lexer::Advance()
{
	if (m_char == '\0')
	{
		return;
	}
	else if (m_char == '\n')
	{
		m_pos.line++;
		m_pos.column = 0;
	}
	else if (m_char == '\r' && m_peek != '\n')
	{
		m_diag << m_pos << Error << LexCrMustBeFollowedByLf << DiagEnd;
		m_pos.line++;
		m_pos.column = 0;
	}

	m_source.Advance();
	m_char = m_source.Cur();
	m_peek = m_source.Peek();
	m_pos.column++;
}

Token &Lexer::MakeWhitespace()
{
	m_token.type = TokWhiteSpace;
	m_token.range.beg = m_pos;
	m_token.slice.beg = m_source.Position();

	while (IsWhitespace(m_char))
	{
		Advance();
	}

	m_token.range.end = m_pos;
	m_token.slice.end = m_source.Position();
	return m_token;
}

Token &Lexer::MakeLineComment()
{
	m_token.type = TokLineComment;
	m_token.range.beg = m_pos;
	m_token.slice.beg = m_source.Position();

	// Skip '//'
	Advance();
	Advance();

	while (!IsEof(m_char) && m_pos.line == m_token.range.beg.line)
	{
		Advance();
	}

	m_token.range.end = m_pos;
	m_token.slice.end = m_source.Position();
	return m_token;
}

Token &Lexer::MakeBlockComment()
{
	m_token.type = TokBlockComment;
	m_token.range.beg = m_pos;
	m_token.slice.beg = m_source.Position();

	// Skip '/*'
	Advance();
	Advance();

	while (true)
	{
		if (IsEof(m_char))
		{
			m_diag
				<< Range(m_token.range.beg, m_pos)
				<< Error
				<< LexUnterminatedBlockComment
				<< DiagEnd;
			break;
		}
		else if (m_char == '/' && m_peek == '*')
		{
			MakeBlockComment();
		}
		else if (m_char == '*' && m_peek == '/')
		{
			// Skip '*/'
			Advance();
			Advance();
			break;
		}

		Advance();
	}

	m_token.range.end = m_pos;
	m_token.slice.end = m_source.Position();
	return m_token;
}

Token &Lexer::MakeIdentifier()
{
	m_token.range.beg = m_pos;
	m_token.slice.beg = m_source.Position();

	while (IsLetter(m_char) || IsDecDigit(m_char) || m_char == '_')
	{
		Advance();
	}

	m_token.range.end = m_pos;
	m_token.slice.end = m_source.Position();
	m_token.type = ClassIdentifier(m_source.GetSlice(m_token.slice));
	return m_token;
}

Token &Lexer::MakeStringLiteral()
{
	m_token.type = TokStringLiteral;
	m_token.range.beg = m_pos;
	m_token.slice.beg = m_source.Position();

	auto start = m_char;
	Advance();

	while (true)
	{
		if (IsEof(m_char))
		{
			m_diag
				<< Range(m_token.range.beg, m_pos)
				<< Error
				<< LexUnterminatedStringLiteral
				<< DiagEnd;
			break;
		}
		else if (m_char == start)
		{
			Advance();
			break;
		}
		else if (m_char == '\\')
		{
			Advance();
		}

		Advance();
	}

	m_token.range.end = m_pos;
	m_token.slice.end = m_source.Position();
	return m_token;
}

Token &Lexer::MakeNumberLiteral()
{
	m_token.type = TokNumberLiteral;
	m_token.range.beg = m_pos;
	m_token.slice.beg = m_source.Position();

	auto base = 10;

	if (m_char == '0')
	{
		if (m_peek == 'b' || m_peek == 'B')
		{
			base = 2;
		}
		else if (m_peek == 'x' || m_peek == 'X')
		{
			base = 16;
		}

		if (base != 10)
		{
			Advance();
			Advance();
		}
	}

	auto empty = true;
	auto invalid = false;
	auto hasDot = base != 10;
	auto hasExp = base != 10;

	while (true)
	{
		if (!IsBaseDigit(m_char, base))
		{
			if (IsHexDigit(m_char))
			{
				invalid = true;
			}
			else
			{
				break;
			}
		}

		empty = false;
		Advance();

		if (m_char == '_' && IsBaseDigit(m_peek, base))
		{
			Advance();
		}

		if (!hasDot && m_char == '.')
		{
			hasDot = true;
			Advance();
		}
		else if (!hasExp && (m_char == 'e' || m_char == 'E'))
		{
			empty = true;
			hasDot = true;
			hasExp = true;
			Advance();
		}
	}

	if (invalid || empty)
	{
		m_diag
			<< Range(m_token.range.beg, m_pos)
			<< Error
			<< LexInvalidNumberLiteral
			<< DiagEnd;
	}

	m_token.range.end = m_pos;
	m_token.slice.end = m_source.Position();
	return m_token;
}

Token &Lexer::MakePunctuation(TokenType type)
{
	m_token.type = type;
	m_token.range = Range(m_pos, 1);
	m_token.slice = Slice(m_source.Position());
	
	// Skip the punctuation character.
	Advance();
	
	return m_token;
}

Token & Mond::Lexer::MakeOperator()
{
	m_token.range.beg = m_pos;
	m_token.slice.beg = m_source.Position();

	// Skip first operator character.
	Advance();

	while (OperatorLookupN(m_char, m_token.type))
	{
		Advance();
	}

	m_token.range.end = m_pos;
	m_token.slice.end = m_source.Position();
	return m_token;
}