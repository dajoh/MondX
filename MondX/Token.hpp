#ifndef MOND_TOKEN_HPP
#define MOND_TOKEN_HPP

#include "Util.hpp"

namespace Mond
{
	enum TokenType
	{
#define MOND_TOKEN(n, s) Tok##n,
#define MOND_PUNCT(n, s) Tok##n,
#define MOND_KEYWORD(n, s) Kw##n,
#define MOND_OPERATOR(n, s) Op##n,
#include "Tokens.inc"
#undef MOND_TOKEN
#undef MOND_PUNCT
#undef MOND_KEYWORD
#undef MOND_OPERATOR
	};

	struct Token
	{
		Range range;
		Slice slice;
		TokenType type;
	};

	TokenType ClassIdentifier(const string &s);
	const char *GetTokenTypeName(TokenType type);
}

#endif