#include "OperatorUtil.hpp"

using namespace Mond;

bool Mond::OperatorLookup1(uint32_t curr, TokenType &type)
{
	switch (curr)
	{
#define MOND_OPERATOR1(c, a) case c: type = Op##a; return true;
#define MOND_OPERATORN(c, a, b)
#include "Tokens.inc"
#undef MOND_OPERATOR1
#undef MOND_OPERATORN
	}

	return false;
}

bool Mond::OperatorLookupN(uint32_t curr, TokenType &type)
{
#define MOND_OPERATOR1(c, a)
#define MOND_OPERATORN(c, a, b) if(type == Op##a && curr == c) { type = Op##b; return true; }
#include "Tokens.inc"
#undef MOND_OPERATOR1
#undef MOND_OPERATORN

	return false;
}