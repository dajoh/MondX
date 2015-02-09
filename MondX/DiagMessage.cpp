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
	case ParseUnterminatedArraySlice:
		return "unterminated array slice";

	case SemaUndeclaredId:
		return "undeclared identifier '%s'";
	case SemaAlreadyDeclared:
		return "'%s' already declared at %d:%d";
	case SemaYieldNotInSequence:
		return "yield can only be used in sequences";
	case SemaLoopControlNotInLoop:
		return "%s can only be used in loops";
	case SemaCaseValueNotConstant:
		return "case value not a constant";
	case SemaDuplicateCaseValue:
		return "duplicate case, already defined at %d:%d";
	case SemaDuplicateDefaultCase:
		return "duplicate default case, already defined at %d:%d";
	case SemaExprNotStorable:
		return "expression not storable";
	case SemaMutatingConstant:
		return "can't change constant '%s' declared at %d:%d";
	}

	throw invalid_argument("unknown diagnostic message");
}