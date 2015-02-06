#include "Sema.hpp"

using namespace Mond;

Sema::Sema(DiagBuilder &diag) : m_diag(diag)
{
}

void Sema::Run(StmtPtrList stmts)
{
	PushScope(Scope::Block);
	{
		for (auto stmt : stmts)
		{
			AcceptChild(this, stmt.get());
		}
	}
	PopScope();
}

void Sema::Visit(ExprFunDecl *expr)
{
	Declare(*expr);

	PushScope(expr->declType == Decl::Function ? Scope::Function : Scope::Sequence);
	{
		Declare(expr->args);
		Visitor::Visit(expr);
	}
	PopScope();
}

void Sema::Visit(ExprId *expr)
{
	for (auto scope : m_scopes)
	{
		auto it = scope.decls.find(expr->name);
		if (it != scope.decls.end())
		{
			return;
		}
	}

	m_diag
		<< expr->range
		<< Error
		<< SemaUndeclaredId
		<< expr->name
		<< DiagEnd;
}

void Sema::Visit(ExprLambda *expr)
{
	PushScope(Scope::Function);
	{
		Declare(expr->args);
		Visitor::Visit(expr);
	}
	PopScope();
}

void Sema::Visit(ExprYield *expr)
{
	if (!IsInSeq())
	{
		m_diag
			<< expr->range
			<< Error
			<< SemaYieldNotInSequence
			<< DiagEnd;
	}
}

void Sema::Visit(ExprListComprehension *expr)
{
	PushScope(Scope::Block);
	{
		for (unsigned int i = 0; i < expr->declNames.size(); i++)
		{
			auto name = expr->declNames[i];
			auto from = expr->generators[i];

			AcceptChild(this, from.get());

			DeclareSubDecl(name, SubDecl(i, *expr));
		}

		for (auto filter : expr->filters)
		{
			AcceptChild(this, filter.get());
		}

		AcceptChild(this, expr->expr.get());
	}
	PopScope();
}

void Sema::Visit(StmtBlock *stmt)
{
	Run(stmt->statements);
}

void Sema::Visit(StmtControl *stmt)
{
	if (!IsInLoop() && (stmt->type == KwBreak || stmt->type == KwContinue))
	{
		m_diag
			<< stmt->range
			<< Error
			<< SemaLoopControlNotInLoop
			<< (stmt->type == KwBreak ? "break" : "continue")
			<< DiagEnd;
	}
}

void Sema::Visit(StmtDoWhile *stmt)
{
	PushScope(Scope::Loop);
	Visitor::Visit(stmt);
	PopScope();
}

void Sema::Visit(StmtFor *stmt)
{
	PushScope(Scope::Loop);
	Visitor::Visit(stmt);
	PopScope();
}

void Sema::Visit(StmtForeach *stmt)
{
	PushScope(Scope::Loop);
	{
		AcceptChild(this, stmt->from.get());
		Declare(*stmt);
		AcceptChild(this, stmt->body.get());
	}
	PopScope();
}

void Sema::Visit(StmtIfElse *stmt)
{
	PushScope(Scope::Block);
	Visitor::Visit(stmt);
	PopScope();
}

void Sema::Visit(StmtVarDecl *stmt)
{
	Declare(*stmt);
	Visitor::Visit(stmt);
}

void Sema::Visit(StmtSwitch *stmt)
{
	PushScope(Scope::Block);
	{
		AcceptChild(this, stmt->value.get());

		for (auto switchCase : stmt->cases)
		{
			// TODO: Check case uniqueness.
			// TODO: Fold values before checking that they're constant.

			if (switchCase.value && !switchCase.value->IsConstant())
			{
				m_diag
					<< switchCase.value->range
					<< Error
					<< SemaCaseValueNotConstant
					<< DiagEnd;
			}

			PushScope(Scope::Block);
			{
				AcceptChild(this, switchCase.value.get());
				Run(switchCase.body);
			}
			PopScope();
		}
	}
	PopScope();
}

void Sema::Visit(StmtWhile *stmt)
{
	PushScope(Scope::Loop);
	Visitor::Visit(stmt);
	PopScope();
}

void Sema::PushScope(Scope::Type type)
{
	auto scope = Scope();
	scope.type = type;
	m_scopes.push_back(scope);
}

void Sema::PopScope()
{
	m_scopes.pop_back();
}

bool Sema::IsInSeq() const
{
	for (int i = m_scopes.size() - 1; i >= 0; i--)
	{
		switch (m_scopes[i].type)
		{
		case Scope::Function:
			return false;
		case Scope::Sequence:
			return true;
		}
	}

	return false;
}

bool Sema::IsInLoop() const
{
	for (int i = m_scopes.size() - 1; i >= 0; i--)
	{
		switch (m_scopes[i].type)
		{
		case Scope::Loop:
			return true;
		case Scope::Function:
		case Scope::Sequence:
			return false;
		}
	}

	return false;
}

void Sema::Declare(Decl decl)
{
	for (unsigned int i = 0; i < decl.declNames.size(); i++)
	{
		DeclareSubDecl(decl.declNames[i], SubDecl(i, decl));
	}
}

void Sema::DeclareSubDecl(const string &name, SubDecl subDecl)
{
	for (auto scope : m_scopes)
	{
		auto it = scope.decls.find(name);
		if (it != scope.decls.end())
		{
			m_diag
				<< GetSubDeclRange(subDecl)
				<< Error
				<< SemaAlreadyDeclared
				<< name
				<< GetSubDeclRange(it->second).beg.line
				<< GetSubDeclRange(it->second).beg.column
				<< DiagEnd;
			return;
		}
	}

	m_scopes.back().decls[name] = subDecl;
}

Range Sema::GetSubDeclRange(SubDecl decl) const
{
	return decl.second.declRanges[decl.first];
}