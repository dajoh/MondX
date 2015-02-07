#ifndef MOND_SEMA_HPP
#define MOND_SEMA_HPP

#include "AST.hpp"
#include "DiagBuilder.hpp"

namespace Mond
{
	struct Decl
	{
		enum Type
		{
			Variable,
			Constant,
			Function,
			Sequence,
			Argument
		};

		Type type;
		Range range;
		AstNode *node;
	};

	typedef unique_ptr<struct Scope> ScopePtr;
	typedef vector<ScopePtr> ScopePtrList;

	struct Scope
	{
		enum Type
		{
			Loop,
			Block,
			Function,
			Sequence
		};

		Type type;
		Scope *parent;
		AstNode *node;
		ScopePtrList children;
		unordered_map<string, Decl> decls;
	};

	class Sema : public Visitor
	{
	public:
		Sema(DiagBuilder &diag);
		~Sema();

		void PushScope(Scope::Type type, AstNode *node);
		void PopScope();

		void Declare(Decl::Type type, Range range, const string &name, AstNode *node);

		virtual void Visit(Expr *);
		virtual void Visit(ExprArrayLiteral *);
		virtual void Visit(ExprArraySlice *);
		virtual void Visit(ExprBinaryOp *);
		virtual void Visit(ExprCall *);
		virtual void Visit(ExprFieldAccess *);
		virtual void Visit(ExprFunDecl *);
		virtual void Visit(ExprId *);
		virtual void Visit(ExprIndexAccess *);
		virtual void Visit(ExprLambda *);
		virtual void Visit(ExprListComprehension *);
		virtual void Visit(ExprNumberLiteral *);
		virtual void Visit(ExprObjectLiteral *);
		virtual void Visit(ExprSimpleLiteral *);
		virtual void Visit(ExprStringLiteral *);
		virtual void Visit(ExprTernaryOp *);
		virtual void Visit(ExprUnaryOp *);
		virtual void Visit(ExprYield *);

		virtual void Visit(Stmt *);
		virtual void Visit(StmtBlock *);
		virtual void Visit(StmtControl *);
		virtual void Visit(StmtDoWhile *);
		virtual void Visit(StmtFor *);
		virtual void Visit(StmtForeach *);
		virtual void Visit(StmtIfElse *);
		virtual void Visit(StmtNakedExpr *);
		virtual void Visit(StmtReturn *);
		virtual void Visit(StmtSwitch *);
		virtual void Visit(StmtVarDecl *);
		virtual void Visit(StmtWhile *);
	private:
		bool IsInSeq() const;
		bool IsInLoop() const;

		Scope *m_scope;
		ScopePtr m_root;
		DiagBuilder &m_diag;
	};

	class SemaScope
	{
	public:
		SemaScope(Sema &sema, Scope::Type type, AstNode *node);
		~SemaScope();
	private:
		Sema &m_sema;
	};

	inline SemaScope::SemaScope(Sema &sema, Scope::Type type, AstNode *node) : m_sema(sema)
	{
		m_sema.PushScope(type, node);
	}

	inline SemaScope::~SemaScope()
	{
		m_sema.PopScope();
	}
}

#endif