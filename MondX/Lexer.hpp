#ifndef MOND_LEXER_HPP
#define MOND_LEXER_HPP

#include "Token.hpp"
#include "Source.hpp"
#include "DiagBuilder.hpp"

namespace Mond
{
	class Lexer
	{
	public:
		Lexer(DiagBuilder &diag, Source &source);

		Token &GetToken();
	private:
		void Advance();

		Token &MakeWhitespace();
		Token &MakeLineComment();
		Token &MakeBlockComment();

		Token &MakeIdentifier();
		Token &MakeStringLiteral();
		Token &MakeNumberLiteral();

		Token &MakePunctuation(TokenType type);
		Token &MakeOperator();

		Pos m_pos;
		Token m_token;
		uint32_t m_char;
		uint32_t m_peek;
		Source &m_source;
		DiagBuilder &m_diag;
	};

	// -------------------------------------------------------------------
	// TODO: Unicode.
	// -------------------------------------------------------------------

	inline bool IsEof(uint32_t c)
	{
		return (c == '\0');
	}

	inline bool IsBinDigit(uint32_t c)
	{
		return (c == '0' || c == '1');
	}

	inline bool IsDecDigit(uint32_t c)
	{
		return (c >= '0' && c <= '9');
	}

	inline bool IsHexDigit(uint32_t c)
	{
		return (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || IsDecDigit(c);
	}

	inline bool IsBaseDigit(uint32_t c, int base)
	{
		switch (base)
		{
		case 2: return IsBinDigit(c);
		case 10: return IsDecDigit(c);
		case 16: return IsHexDigit(c);
		}

		throw invalid_argument("invalid number base");
	}

	inline bool IsLetter(uint32_t c)
	{
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	}

	inline bool IsWhitespace(uint32_t c)
	{
		return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
	}
}

#endif