#ifndef MOND_AST_HPP
#define MOND_AST_HPP

#include "Util.hpp"
#include "Token.hpp"
#include "Visitor.hpp"

namespace Mond
{
	struct AstNode
	{
		virtual ~AstNode() {}
		virtual void Accept(Visitor *) = 0;

		Pos pos;
		Range range;
	};

	struct Expr : public AstNode
	{
		virtual bool WantsSemi() { return true; }
		virtual bool IsConstant() { return false; }
	};

	struct Stmt : public AstNode
	{
	};

	struct Decl
	{
		enum Type
		{
			Variable,
			Constant,
			Function,
			Sequence
		};

		Type declType;
		vector<Range> declRanges;
		vector<string> declNames;
	};

	typedef shared_ptr<Expr> ExprPtr;
	typedef vector<ExprPtr> ExprPtrList;

	typedef shared_ptr<Stmt> StmtPtr;
	typedef vector<StmtPtr> StmtPtrList;

	// --------------------------------------------------------------------------
	// Expressions
	// --------------------------------------------------------------------------

	struct ExprArrayLiteral : public Expr
	{
		void Accept(Visitor *v) { v->Visit(this); }

		ExprPtrList elems;
	};

	struct ExprArraySlice : public Expr
	{
		void Accept(Visitor *v) { v->Visit(this); }

		ExprPtr left;
		ExprPtr start;
		ExprPtr end;
		ExprPtr step;
	};

	struct ExprBinaryOp : public Expr
	{
		void Accept(Visitor *v) { v->Visit(this); }

		ExprPtr left;
		ExprPtr right;
		TokenType type;
	};

	struct ExprCall : public Expr
	{
		void Accept(Visitor *v) { v->Visit(this); }

		ExprPtr left;
		ExprPtrList args;
	};

	struct ExprFieldAccess : public Expr
	{
		void Accept(Visitor *v) { v->Visit(this); }

		string name;
		ExprPtr left;
	};

	struct ExprId : public Expr
	{
		void Accept(Visitor *v) { v->Visit(this); }

		string name;
	};

	struct ExprIndexAccess : public Expr
	{
		void Accept(Visitor *v) { v->Visit(this); }

		ExprPtr left;
		ExprPtr index;
	};

	struct ExprLambda : public Expr
	{
		void Accept(Visitor *v) { v->Visit(this); }

		bool varargs;
		Decl args;
		StmtPtr body;
	};

	struct ExprFunDecl : public ExprLambda, public Decl
	{
		void Accept(Visitor *v) { v->Visit(this); }
		bool WantsSemi() { return semi; }

		bool semi;
	};

	struct ExprListComprehension : public Expr, public Decl
	{
		void Accept(Visitor *v) { v->Visit(this); }

		ExprPtr expr;
		ExprPtrList filters;
		ExprPtrList generators;
	};

	struct ExprNumberLiteral : public Expr
	{
		void Accept(Visitor *v) { v->Visit(this); }
		bool IsConstant() { return true; }

		double value;
	};

	struct ExprObjectLiteral : public Expr
	{
		void Accept(Visitor *v) { v->Visit(this); }

		struct KeyValue
		{
			string key;
			ExprPtr value;
		};

		ExprPtrList fnEntries;
		vector<KeyValue> kvEntries;
	};

	struct ExprSimpleLiteral : public Expr
	{
		void Accept(Visitor *v) { v->Visit(this); }
		bool IsConstant() { return true; }

		TokenType type;
	};

	struct ExprStringLiteral : public Expr
	{
		void Accept(Visitor *v) { v->Visit(this); }
		bool IsConstant() { return true; }

		string contents;
	};

	struct ExprTernaryOp : public Expr
	{
		void Accept(Visitor *v) { v->Visit(this); }

		ExprPtr cond;
		ExprPtr thenExpr;
		ExprPtr elseExpr;
	};

	struct ExprUnaryOp : public Expr
	{
		void Accept(Visitor *v) { v->Visit(this); }

		bool post;
		ExprPtr value;
		TokenType op;
	};

	struct ExprYield : public Expr
	{
		void Accept(Visitor *v) { v->Visit(this); }

		ExprPtr value;
	};

	// -------------------------------------------------
	// Statements
	// -------------------------------------------------

	struct StmtBlock : public Stmt
	{
		void Accept(Visitor *v) { v->Visit(this); }

		StmtPtrList statements;
	};

	struct StmtControl : public Stmt
	{
		void Accept(Visitor *v) { v->Visit(this); }

		TokenType type;
	};

	struct StmtDoWhile : public Stmt
	{
		void Accept(Visitor *v) { v->Visit(this); }

		StmtPtr body;
		ExprPtr cond;
	};

	struct StmtFor : public Stmt
	{
		void Accept(Visitor *v) { v->Visit(this); }

		StmtPtr init;
		ExprPtr cond;
		ExprPtrList steps;
		StmtPtr body;
	};

	struct StmtForeach : public Stmt, public Decl
	{
		void Accept(Visitor *v) { v->Visit(this); }

		ExprPtr from;
		StmtPtr body;
	};

	struct StmtIfElse : public Stmt
	{
		void Accept(Visitor *v) { v->Visit(this); }

		ExprPtr cond;
		StmtPtr thenBody;
		StmtPtr elseBody;
	};

	struct StmtNakedExpr : public Stmt
	{
		void Accept(Visitor *v) { v->Visit(this); }

		ExprPtr value;
	};

	struct StmtReturn : public Stmt
	{
		void Accept(Visitor *v) { v->Visit(this); }

		ExprPtr value;
	};

	struct StmtSwitch : public Stmt
	{
		void Accept(Visitor *v) { v->Visit(this); }

		struct Case
		{
			bool def;
			Range headRange;
			ExprPtr value;
			StmtPtrList body;
		};

		ExprPtr value;
		vector<Case> cases;
	};

	struct StmtVarDecl : public Stmt, public Decl
	{
		void Accept(Visitor *v) { v->Visit(this); }

		ExprPtrList values;
	};

	struct StmtWhile : public Stmt
	{
		void Accept(Visitor *v) { v->Visit(this); }

		ExprPtr cond;
		StmtPtr body;
	};
}

#endif
