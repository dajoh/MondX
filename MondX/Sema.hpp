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
		AstNodePtr node;
	};

	typedef shared_ptr<struct Scope> ScopePtr;
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
		AstNodePtr node;
		ScopePtrList children;
		unordered_map<string, Decl> decls;
	};

	class Sema : public Visitor
	{
	public:
		Sema(DiagBuilder &diag, ScopePtr builtinScope);
		~Sema();

		ScopePtr RootScope() const;

		void PushScope(Scope::Type type, AstNodePtr node);
		void PopScope();

		void Declare(Decl::Type type, Range range, const string &name, AstNodePtr node);

		virtual void Visit(Expr *);
		virtual void Visit(ExprArrayLiteral *);
		virtual void Visit(ExprArraySlice *);
		virtual void Visit(ExprBinaryOp *);
		virtual void Visit(ExprCall *);
		virtual void Visit(ExprFieldAccess *);
		virtual void Visit(ExprId *);
		virtual void Visit(ExprIndexAccess *);
		virtual void Visit(ExprLambda *);
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
		virtual void Visit(StmtFunDecl *);
		virtual void Visit(StmtIfElse *);
		virtual void Visit(StmtNakedExpr *);
		virtual void Visit(StmtReturn *);
		virtual void Visit(StmtSwitch *);
		virtual void Visit(StmtVarDecl *);
		virtual void Visit(StmtWhile *);
	private:
		bool IsInSeq() const;
		bool IsInLoop() const;
		void CheckMutable(Expr *expr) const;

		Decl *FindDecl(const string &name) const;

		Scope *m_curr;
		ScopePtr m_root;
		ScopePtr m_builtin;
		DiagBuilder &m_diag;
	};

	class SemaScope
	{
	public:
		SemaScope(Sema &sema, Scope::Type type, AstNodePtr node);
		~SemaScope();
	private:
		Sema &m_sema;
	};

	inline SemaScope::SemaScope(Sema &sema, Scope::Type type, AstNodePtr node) : m_sema(sema)
	{
		m_sema.PushScope(type, node);
	}

	inline SemaScope::~SemaScope()
	{
		m_sema.PopScope();
	}
}

#endif
