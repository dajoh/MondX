#ifndef MOND_SEMA_HPP
#define MOND_SEMA_HPP

#include "AST.hpp"
#include "DiagBuilder.hpp"

namespace Mond
{
	class Sema : public Visitor
	{
	public:
		Sema(DiagBuilder &diag);

		void Run(StmtPtrList stmts);

		virtual void Visit(ExprFunDecl *);
		virtual void Visit(ExprId *);
		virtual void Visit(ExprLambda *);
		virtual void Visit(ExprListComprehension *);
		virtual void Visit(ExprYield *);

		virtual void Visit(StmtBlock *);
		virtual void Visit(StmtControl *);
		virtual void Visit(StmtDoWhile *);
		virtual void Visit(StmtFor *);
		virtual void Visit(StmtForeach *);
		virtual void Visit(StmtIfElse *);
		virtual void Visit(StmtVarDecl *);
		virtual void Visit(StmtSwitch *);
		virtual void Visit(StmtWhile *);
	private:
		typedef pair<int, Decl> SubDecl;

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
			unordered_map<string, SubDecl> decls;
		};

		void PushScope(Scope::Type type);
		void PopScope();

		bool IsInSeq() const;
		bool IsInLoop() const;

		void Declare(Decl decl);
		void DeclareSubDecl(const string &name, SubDecl subDecl);
		Range GetSubDeclRange(SubDecl decl) const;

		DiagBuilder &m_diag;
		vector<Scope> m_scopes;
	};
}

#endif