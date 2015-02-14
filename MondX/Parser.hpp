#ifndef MOND_PARSER_HPP
#define MOND_PARSER_HPP

#include "Sema.hpp"
#include "Lexer.hpp"
#include "OperatorUtil.hpp"

namespace Mond
{
	class Parser
	{
	public:
		Parser(DiagBuilder &diag, Source &source, Lexer &lexer, Sema &sema);

		StmtPtr ParseFile();

		ExprPtr ParseExpr();
		StmtPtr ParseStmt();
	private:
		void More();
		void Advance();

		Token EatToken();
		Token EatToken(TokenType type);
		Token Lookahead(int n = 0);
		Token CreateMissing(TokenType type, bool error);

		string IdString(Slice s);
		string LiteralString(Slice s);
		double LiteralNumber(Slice s);

		// -------------------------------------------------------------------
		// Expressions
		// -------------------------------------------------------------------

		bool CanBeExpr();

		ExprPtr ParseExprCore(Precedence p);

		ExprPtr ParseExprId();
		ExprPtr ParseExprStringLiteral();
		ExprPtr ParseExprNumberLiteral();
		ExprPtr ParseExprSimpleLiteral();

		ExprPtr ParseExprParens();
		ExprPtr ParseExprObjectLiteral();
		ExprPtr ParseExprArrayLiteral();

		ExprPtr ParseExprYield();

		ExprPtr ParseExprCall(ExprPtr left);
		ExprPtr ParseExprIndexAccess(ExprPtr left);
		ExprPtr ParseExprFieldAccess(ExprPtr left);

		ExprPtr ParseExprPrefixOp();
		ExprPtr ParseExprPostfixOp(ExprPtr left);
		ExprPtr ParseExprBinaryOp(ExprPtr left, Precedence p);
		ExprPtr ParseExprTernaryOp(ExprPtr left);

		ExprPtr ParseExprLambda();
		ExprPtr ParseExprCondition();
		ExprPtr ParseExprArraySlice(Pos pos, ExprPtr left, ExprPtr first);

		// -------------------------------------------------------------------
		// Statements
		// -------------------------------------------------------------------

		StmtPtr ParseStmtCore();

		StmtPtr ParseStmtBlock();
		StmtPtr ParseStmtControl();
		StmtPtr ParseStmtDoWhile();
		StmtPtr ParseStmtFor();
		StmtPtr ParseStmtForeach();
		StmtPtr ParseStmtFunDecl();
		StmtPtr ParseStmtIfElse();
		StmtPtr ParseStmtReturn();
		StmtPtr ParseStmtVarDecl();
		StmtPtr ParseStmtSwitch();
		StmtPtr ParseStmtWhile();

		StmtPtr ParseStmtNakedExpr();
		StmtPtr ParseStmtLambdaBody(bool isShorthand);

		// -------------------------------------------------------------------
		// Reused parsers
		// -------------------------------------------------------------------

		Pos ParseTerminator(TokenType type, Pos beg, DiagMessage msg);
		void ParseArgumentList(bool &varargs);
	private:
		Sema &m_sema;
		Lexer &m_lexer;
		Source &m_source;
		DiagBuilder &m_diag;

		Token m_token;
		deque<Token> m_lookahead;
	};
}

#endif
