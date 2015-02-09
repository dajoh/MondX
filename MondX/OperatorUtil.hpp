#ifndef MOND_OPERATOR_UTIL_HPP
#define MOND_OPERATOR_UTIL_HPP

#include "Token.hpp"

namespace Mond
{
	enum class Precedence
	{
		Invalid,
		Assign,
		Ternary,
		ConditionalOr,
		ConditionalAnd,
		Equality,
		Relational,
		BitOr,
		BitXor,
		BitAnd,
		BitShift,
		Addition,
		Multiplication,
		Misc
	};

	bool OperatorLookup1(uint32_t curr, TokenType &type);
	bool OperatorLookupN(uint32_t curr, TokenType &type);

	bool IsPrefixOperator(TokenType op);
	bool IsBinaryOperator(TokenType op);
	bool IsPostfixOperator(TokenType op);
	bool IsMutatingOperator(TokenType op);

	Precedence GetOperatorPrecedence(TokenType op);
}

#endif