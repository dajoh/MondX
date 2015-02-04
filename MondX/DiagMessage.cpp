#include "Util.hpp"
#include "DiagMessage.hpp"

using namespace Mond;

const char *Mond::GetDiagMessageFormat(DiagMessage msg)
{
	switch (msg)
	{
	case LexUnexpectedCharacter:
		return "unexpected character '%c'";
	case LexCrMustBeFollowedByLf:
		return "carriage return must be followed by line feed";
	case LexInvalidNumberLiteral:
		return "invalid number literal";
	case LexUnterminatedBlockComment:
		return "unterminated block comment";
	case LexUnterminatedStringLiteral:
		return "unterminated string literal";

	case ParseExpectedExpr:
		return "expected expression";
	case ParseExpectedStmt:
		return "expected statement";
	case ParseExpectedSwitchCase:
		return "expected switch case";
	case ParseExpectedObjectEntry:
		return "expected object entry";
	case ParseExpectedFilterOrGenerator:
		return "expected filter or generator";

	case ParseMismatchedToken:
		return "mismatched '%t'";
	case ParseExpectedTokenGotOther:
		return "expected '%t', got '%t'";

	case ParseConstNotInitialized:
		return "constant not initialized";
	case ParseUnnecessaryPointyInFun:
		return "unnecessary '->'";
	case ParseUnterminatedArrayLiteral:
		return "unterminated array literal";
	case ParseUnterminatedObjectLiteral:
		return "unterminated object literal";
	case ParseUnterminatedFunctionCall:
		return "unterminated function call";
	case ParseUnterminatedListComprehension:
		return "unterminated list comprehension";
	}

	throw invalid_argument("unknown diagnostic message");
}