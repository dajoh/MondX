#ifndef MOND_AST_HPP
#define MOND_AST_HPP

#include "Util.hpp"
#include "Token.hpp"
#include "Visitor.hpp"

namespace Mond
{
	struct AstNode
	{
		Pos pos;
		Range range;
	};

	struct Expr : public AstNode
	{
		virtual ~Expr() {}
		virtual void Accept(Visitor *) = 0;
		virtual bool WantsSemi() { return true; }
	};

	struct Stmt : public AstNode
	{
		virtual ~Stmt() {}
		virtual void Accept(Visitor *) = 0;
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
		StmtPtr body;
		vector<string> args;
	};

	struct ExprFunDecl : public ExprLambda
	{
		void Accept(Visitor *v) { v->Visit(this); }
		bool WantsSemi() { return semi; }

		bool fun;
		bool semi;
		string name;
	};

	struct ExprListComprehension : public Expr
	{
		void Accept(Visitor *v) { v->Visit(this); }

		struct Generator
		{
			string name;
			ExprPtr from;
		};

		ExprPtr expr;
		ExprPtrList filters;
		vector<Generator> generators;
	};

	struct ExprNumberLiteral : public Expr
	{
		void Accept(Visitor *v) { v->Visit(this); }

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

		TokenType type;
	};

	struct ExprStringLiteral : public Expr
	{
		void Accept(Visitor *v) { v->Visit(this); }

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

	struct StmtForeach : public Stmt
	{
		void Accept(Visitor *v) { v->Visit(this); }

		Slice name;
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
			ExprPtr cond;
			StmtPtrList body;
		};

		ExprPtr value;
		vector<Case> cases;
	};

	struct StmtVarDecl : public Stmt
	{
		void Accept(Visitor *v) { v->Visit(this); }

		struct Decl
		{
			string name;
			ExprPtr value;
		};

		bool var;
		vector<Decl> decls;
	};

	struct StmtWhile : public Stmt
	{
		void Accept(Visitor *v) { v->Visit(this); }

		ExprPtr cond;
		StmtPtr body;
	};
}

#endif
