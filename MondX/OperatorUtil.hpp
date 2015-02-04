#ifndef MOND_OPERATOR_UTIL_HPP
#define MOND_OPERATOR_UTIL_HPP

#include "Token.hpp"

namespace Mond
{
	bool OperatorLookup1(uint32_t curr, TokenType &type);
	bool OperatorLookupN(uint32_t curr, TokenType &type);
}

#endif