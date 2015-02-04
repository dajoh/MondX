#include "Token.hpp"

using namespace Mond;

TokenType Mond::ClassIdentifier(const string &s)
{
	static unordered_map<string, TokenType> keywords =
	{
#define MOND_KEYWORD(n, s) { s, Kw##n },
#include "Tokens.inc"
#undef MOND_KEYWORD
	};

	auto it = keywords.find(s);
	return it == keywords.end() ? TokIdentifier : it->second;
}

const char *Mond::GetTokenTypeName(TokenType type)
{
	switch (type)
	{
#define MOND_TOKEN(n, s) case Tok##n: return s; break;
#define MOND_PUNCT(n, s) case Tok##n: return s; break;
#define MOND_KEYWORD(n, s) case Kw##n: return s; break;
#define MOND_OPERATOR(n, s) case Op##n: return s; break;
#include "Tokens.inc"
#undef MOND_TOKEN
#undef MOND_PUNCT
#undef MOND_KEYWORD
#undef MOND_OPERATOR
	}

	throw invalid_argument("invalid token type");
}