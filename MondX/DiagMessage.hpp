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

		ParseMismatchedToken,
		ParseExpectedTokenGotOther,

		ParseConstNotInitialized,
		ParseUnnecessaryPointyInFun,
		ParseUnterminatedArrayLiteral,
		ParseUnterminatedObjectLiteral,
		ParseUnterminatedFunctionCall,
		ParseUnterminatedArraySlice,

		SemaUndeclaredId,
		SemaAlreadyDeclared,
		SemaAlreadyDeclaredAt,

		SemaYieldNotInSequence,
		SemaLoopControlNotInLoop,
		SemaCaseValueNotConstant,
		SemaDuplicateCaseValue,
		SemaDuplicateDefaultCase,

		SemaExprNotStorable,
		SemaMutatingConstant,
	};

	const char *GetDiagMessageFormat(DiagMessage msg);
}

#endif