#include "Sema.hpp"

using namespace Mond;

Sema::Sema(DiagBuilder &diag) : m_root(new Scope()), m_diag(diag)
{
	m_root->type = Scope::Block;
	m_root->node = nullptr;
	m_root->parent = nullptr;

	m_scope = m_root.get();
}

Sema::~Sema()
{
}

void Sema::PushScope(Scope::Type type, AstNode *node)
{
	m_scope->children.push_back(ScopePtr(new Scope()));

	auto &scope = m_scope->children.back();
	scope->type = type;
	scope->node = node;
	scope->parent = m_scope;

	m_scope = scope.get();
}

void Sema::PopScope()
{
	m_scope = m_scope->parent;
}

void Sema::Declare(Decl::Type type, Range range, const string &name, AstNode *node)
{
	Scope *scope = m_scope;
	while (scope != nullptr)
	{
		auto it = scope->decls.find(name);
		if (it != scope->decls.end())
		{
			m_diag
				<< range
				<< Error
				<< SemaAlreadyDeclared
				<< name
				<< it->second.range.beg.line
				<< it->second.range.beg.column
				<< DiagEnd;
			break;
		}

		scope = scope->parent;
	}

	Decl decl;
	decl.type = type;
	decl.range = range;
	decl.node = node;
	m_scope->decls[name] = decl;
}

void Sema::Visit(Expr *)
{
	throw logic_error("unreachable in sema visit expr");
}

void Sema::Visit(ExprArrayLiteral *)
{
}

void Sema::Visit(ExprArraySlice *)
{
}

void Sema::Visit(ExprBinaryOp *)
{
}

void Sema::Visit(ExprCall *)
{
}

void Sema::Visit(ExprFieldAccess *)
{
}

void Sema::Visit(ExprFunDecl *)
{
}

void Sema::Visit(ExprId *expr)
{
	Scope *scope = m_scope;
	while (scope != nullptr)
	{
		if (scope->decls.count(expr->name) != 0)
		{
			return;
		}

		scope = scope->parent;
	}

	m_diag
		<< expr->range
		<< Error
		<< SemaUndeclaredId
		<< expr->name
		<< DiagEnd;
}

void Sema::Visit(ExprIndexAccess *)
{
}

void Sema::Visit(ExprLambda *)
{
}

void Sema::Visit(ExprListComprehension *)
{
}

void Sema::Visit(ExprNumberLiteral *)
{
}

void Sema::Visit(ExprObjectLiteral *)
{
}

void Sema::Visit(ExprSimpleLiteral *)
{
}

void Sema::Visit(ExprStringLiteral *)
{
}

void Sema::Visit(ExprTernaryOp *)
{
}

void Sema::Visit(ExprUnaryOp *)
{
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

void Sema::Visit(Stmt *)
{
	throw logic_error("unreachable in sema visit stmt");
}

void Sema::Visit(StmtBlock *)
{
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

void Sema::Visit(StmtDoWhile *)
{
}

void Sema::Visit(StmtFor *)
{
}

void Sema::Visit(StmtForeach *)
{
}

void Sema::Visit(StmtIfElse *)
{
}

void Sema::Visit(StmtNakedExpr *)
{
}

void Sema::Visit(StmtReturn *)
{
}

void Sema::Visit(StmtSwitch *stmt)
{
	Pos defaultPos;

	for (auto &switchCase : stmt->cases)
	{
		// TODO: Check case uniqueness.
		// TODO: Fold values before checking that they're constant.

		if (switchCase.def && defaultPos.IsValid())
		{
			m_diag
				<< switchCase.headRange
				<< Error
				<< SemaDuplicateDefaultCase
				<< defaultPos.line
				<< defaultPos.column
				<< DiagEnd;
		}
		else if (switchCase.value && !switchCase.value->IsConstant())
		{
			m_diag
				<< switchCase.value->range
				<< Error
				<< SemaCaseValueNotConstant
				<< DiagEnd;
		}

		if (!defaultPos.IsValid() && switchCase.def)
		{
			defaultPos = switchCase.headRange.beg;
		}
	}
}

void Sema::Visit(StmtVarDecl *)
{
}

void Sema::Visit(StmtWhile *)
{
}

bool Sema::IsInSeq() const
{
	Scope *scope = m_scope;
	while (scope != nullptr)
	{
		switch (scope->type)
		{
		case Scope::Function:
			return false;
		case Scope::Sequence:
			return true;
		default:
			break;
		}

		scope = scope->parent;
	}

	return false;
}

bool Sema::IsInLoop() const
{
	Scope *scope = m_scope;
	while (scope != nullptr)
	{
		switch (scope->type)
		{
		case Scope::Loop:
			return true;
		case Scope::Function:
		case Scope::Sequence:
			return false;
		default:
			break;
		}

		scope = scope->parent;
	}

	return false;
}
