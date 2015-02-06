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
		ParseExpectedSwitchCase,
		ParseExpectedObjectEntry,
		ParseExpectedFilterOrGenerator,

		ParseMismatchedToken,
		ParseExpectedTokenGotOther,

		ParseConstNotInitialized,
		ParseUnnecessaryPointyInFun,
		ParseUnterminatedArrayLiteral,
		ParseUnterminatedObjectLiteral,
		ParseUnterminatedFunctionCall,
		ParseUnterminatedListComprehension,

		SemaUndeclaredId,
		SemaAlreadyDeclared,
		SemaYieldNotInSequence,
		SemaLoopControlNotInLoop,
		SemaCaseValueNotConstant,
		SemaDuplicateCaseValue,
		SemaDuplicateDefaultCase,
	};

	const char *GetDiagMessageFormat(DiagMessage msg);
}

#endif