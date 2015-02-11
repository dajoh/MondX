#include "Sema.hpp"
#include "OperatorUtil.hpp"

using namespace Mond;

Sema::Sema(DiagBuilder &diag, ScopePtr builtinScope) :
	m_root(new Scope()),
	m_builtin(builtinScope),
	m_diag(diag)
{
	m_curr = m_root.get();
	m_curr->type = Scope::Block;
	m_curr->node = NULL;
	m_curr->parent = builtinScope.get();
}

Sema::~Sema()
{
}

ScopePtr Sema::RootScope() const
{
	return m_root;
}

void Sema::PushScope(Scope::Type type, AstNode *node)
{
	ScopePtr newScope(new Scope());
	newScope->type = type;
	newScope->node = node;
	newScope->parent = m_curr;

	m_curr->children.push_back(newScope);
	m_curr = newScope.get();
}

void Sema::PopScope()
{
	m_curr = m_curr->parent;
}

void Sema::Declare(Decl::Type type, Range range, const string &name, AstNode *node)
{
	bool builtin = false;
	Scope *scope = m_curr;
	do
	{
		auto it = scope->decls.find(name);
		if (it != scope->decls.end())
		{
			if (!builtin)
			{
				m_diag
					<< range
					<< Error
					<< SemaAlreadyDeclaredAt
					<< name
					<< it->second.range.beg.line
					<< it->second.range.beg.column
					<< DiagEnd;
			}
			else
			{
				m_diag
					<< range
					<< Error
					<< SemaAlreadyDeclared
					<< name
					<< DiagEnd;
			}

			break;
		}

		scope = scope->parent;
		if (scope == m_builtin.get())
		{
			builtin = true;
		}
	} while (scope != NULL);

	Decl decl;
	decl.type = type;
	decl.range = range;
	decl.node = node;
	m_curr->decls[name] = decl;
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

void Sema::Visit(ExprBinaryOp *expr)
{
	if (IsMutatingOperator(expr->type) && expr->left)
	{
		CheckMutable(expr->left.get());
	}
}

void Sema::Visit(ExprCall *)
{
}

void Sema::Visit(ExprFieldAccess *)
{
}

void Sema::Visit(ExprId *expr)
{
	Decl *decl = FindDecl(expr->name);
	if (!decl)
	{
		m_diag
			<< expr->range
			<< Error
			<< SemaUndeclaredId
			<< expr->name
			<< DiagEnd;
	}
}

void Sema::Visit(ExprIndexAccess *)
{
}

void Sema::Visit(ExprLambda *)
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

void Sema::Visit(ExprUnaryOp *expr)
{
	if (IsMutatingOperator(expr->type) && expr->value)
	{
		CheckMutable(expr->value.get());
	}
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

void Sema::Visit(StmtFunDecl *)
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
	Scope *scope = m_curr;
	do
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
	} while (scope != NULL);

	return false;
}

bool Sema::IsInLoop() const
{
	Scope *scope = m_curr;
	do
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
	} while (scope != NULL);

	return false;
}

void Sema::CheckMutable(Expr *expr) const
{
	if (!expr->IsStorable())
	{
		m_diag
			<< expr->range
			<< Error
			<< SemaExprNotStorable
			<< DiagEnd;
		return;
	}

	auto id = dynamic_cast<ExprId *>(expr);
	if (!id)
	{
		return;
	}

	auto decl = FindDecl(id->name);
	if (decl && decl->type == Decl::Constant)
	{
		m_diag
			<< id->range
			<< Error
			<< SemaMutatingConstant
			<< id->name
			<< decl->range.beg.line
			<< decl->range.beg.column
			<< DiagEnd;
	}
}

Decl *Sema::FindDecl(const string &name) const
{
	Scope *scope = m_curr;
	do
	{
		auto it = scope->decls.find(name);
		if (it != scope->decls.end())
		{
			return &it->second;
		}

		scope = scope->parent;
	} while (scope != NULL);

	return NULL;
}
