#ifndef MOND_PARSER_HPP
#define MOND_PARSER_HPP

#include "AST.hpp"
#include "Lexer.hpp"

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

	class Parser
	{
	public:
		Parser(DiagBuilder &diag, Source &source, Lexer &lexer);

		StmtPtrList ParseFile();

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

		Expr *ParseExprCore(Precedence p);

		Expr *ParseExprId();
		Expr *ParseExprStringLiteral();
		Expr *ParseExprNumberLiteral();
		Expr *ParseExprSimpleLiteral();

		Expr *ParseExprParens();
		Expr *ParseExprObjectLiteral();
		Expr *ParseExprArrayLiteral();

		Expr *ParseExprFunDecl();
		Expr *ParseExprYield();

		Expr *ParseExprCall(Expr *left);
		Expr *ParseExprIndexAccess(Expr *left);
		Expr *ParseExprFieldAccess(Expr *left);

		Expr *ParseExprPrefixOp();
		Expr *ParseExprPostfixOp(Expr *left);
		Expr *ParseExprBinaryOp(Expr *left, Precedence p);
		Expr *ParseExprTernaryOp(Expr *left);

		Expr *ParseExprLambda();
		Expr *ParseExprCondition();
		Expr *ParseExprArraySlice(Pos pos, Expr *left, Expr *first);
		Expr *ParseExprListComprehension(Pos pos, Expr *first);

		// -------------------------------------------------------------------
		// Statements
		// -------------------------------------------------------------------

		Stmt *ParseStmtCore();

		Stmt *ParseStmtBlock();
		Stmt *ParseStmtControl();
		Stmt *ParseStmtDoWhile();
		Stmt *ParseStmtFor();
		Stmt *ParseStmtForeach();
		Stmt *ParseStmtIfElse();
		Stmt *ParseStmtReturn();
		Stmt *ParseStmtVarDecl();
		Stmt *ParseStmtSwitch();
		Stmt *ParseStmtWhile();

		Stmt *ParseStmtNakedExpr();
		Stmt *ParseStmtLambdaBody();

		// -------------------------------------------------------------------
		// Reused parsers
		// -------------------------------------------------------------------

		Pos ParseTerminator(TokenType type, Pos beg, DiagMessage msg);
		Decl ParseArgumentList(bool &varargs);

		// -------------------------------------------------------------------
		// Operators
		// -------------------------------------------------------------------

		bool IsPrefixOperator() const;
		bool IsBinaryOperator() const;
		bool IsPostfixOperator() const;

		Precedence GetOperatorPrecedence();
	private:
		Lexer &m_lexer;
		Source &m_source;
		DiagBuilder &m_diag;

		Token m_token;
		deque<Token> m_lookahead;
	};
}

#endif
