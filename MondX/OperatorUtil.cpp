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

bool Mond::IsPrefixOperator(TokenType op)
{
	switch (op)
	{
	case OpSubtract:
	case OpIncrement:
	case OpDecrement:
	case OpBitNot:
	case OpNot:
	case OpEllipsis:
		return true;
	default:
		return false;
	}
}

bool Mond::IsBinaryOperator(TokenType op)
{
	switch (op)
	{
	case OpAdd:
	case OpSubtract:
	case OpMultiply:
	case OpDivide:
	case OpModulo:
	case OpExponent:
	case OpBitLeftShift:
	case OpBitRightShift:
	case OpBitAnd:
	case OpBitOr:
	case OpBitXor:

	case OpAssign:
	case OpAddAssign:
	case OpSubtractAssign:
	case OpMultiplyAssign:
	case OpDivideAssign:
	case OpModuloAssign:
	case OpExponentAssign:
	case OpBitLeftShiftAssign:
	case OpBitRightShiftAssign:
	case OpBitAndAssign:
	case OpBitOrAssign:
	case OpBitXorAssign:

	case OpEqualTo:
	case OpNotEqualTo:
	case OpGreaterThan:
	case OpGreaterThanOrEqual:
	case OpLessThan:
	case OpLessThanOrEqual:
	case OpConditionalAnd:
	case OpConditionalOr:
	case KwIn:
	case KwNotIn:

	case OpPipeline:
		return true;

	default:
		return false;
	}
}

bool Mond::IsPostfixOperator(TokenType op)
{
	switch (op)
	{
	case OpIncrement:
	case OpDecrement:
		return true;
	default:
		return false;
	}
}

bool Mond::IsMutatingOperator(TokenType op)
{
	switch (op)
	{
	case OpIncrement:
	case OpDecrement:

	case OpAssign:
	case OpAddAssign:
	case OpSubtractAssign:
	case OpMultiplyAssign:
	case OpDivideAssign:
	case OpModuloAssign:
	case OpExponentAssign:
	case OpBitLeftShiftAssign:
	case OpBitRightShiftAssign:
	case OpBitAndAssign:
	case OpBitOrAssign:
	case OpBitXorAssign:
		return true;

	default:
		return false;
	}
}

Precedence Mond::GetOperatorPrecedence(TokenType op)
{
	switch (op)
	{
	case OpAssign:
	case OpAddAssign:
	case OpSubtractAssign:
	case OpMultiplyAssign:
	case OpDivideAssign:
	case OpModuloAssign:
	case OpExponentAssign:
	case OpBitLeftShiftAssign:
	case OpBitRightShiftAssign:
	case OpBitAndAssign:
	case OpBitOrAssign:
	case OpBitXorAssign:
		return Precedence::Assign;

	case OpQuestionMark:
		return Precedence::Ternary;

	case OpConditionalOr:
		return Precedence::ConditionalOr;

	case OpConditionalAnd:
		return Precedence::ConditionalAnd;

	case OpEqualTo:
	case OpNotEqualTo:
		return Precedence::Equality;

	case OpGreaterThan:
	case OpGreaterThanOrEqual:
	case OpLessThan:
	case OpLessThanOrEqual:
	case KwIn:
	case KwNotIn:
		return Precedence::Relational;

	case OpBitOr:
		return Precedence::BitOr;
	case OpBitXor:
		return Precedence::BitXor;
	case OpBitAnd:
		return Precedence::BitAnd;

	case OpBitLeftShift:
	case OpBitRightShift:
		return Precedence::BitShift;

	case OpAdd:
	case OpSubtract:
		return Precedence::Addition;

	case OpMultiply:
	case OpDivide:
	case OpModulo:
	case OpExponent:
		return Precedence::Multiplication;

	case OpPipeline:
		return Precedence::Misc;

	default:
		throw logic_error("precedence requested for unknown operator");
	}
}
