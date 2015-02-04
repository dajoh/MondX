#ifndef MOND_DIAG_MESSAGE_HPP
#define MOND_DIAG_MESSAGE_HPP

namespace Mond
{
	enum DiagMessage
	{
		LexUnexpectedCharacter,
		LexCrMustBeFollowedByLf,
		LexInvalidNumberLiteral,
		LexUnterminatedBlockComment,
		LexUnterminatedStringLiteral,

		ParseExpectedExpr,
		ParseExpectedStmt,
		ParseExpectedObjectEntry,

		ParseMismatchedToken,
		ParseExpectedTokenGotOther,

		ParseConstNotInitialized,
		ParseUnnecessaryPointyInFun,
		ParseUnterminatedArrayLiteral,
		ParseUnterminatedObjectLiteral,
		ParseUnterminatedFunctionCall,
		ParseUnterminatedListComprehension,
	};

	const char *GetDiagMessageFormat(DiagMessage msg);
}

#endif