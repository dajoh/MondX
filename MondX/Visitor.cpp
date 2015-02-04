#include "AST.hpp"

using namespace Mond;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

template<class T>
void VisitSelf(Visitor *v, T t)
{
	v->Visit((Expr *)t);
}

template<class T>
void AcceptChild(Visitor *v, T t)
{
	if (t)
	{
		t->Accept(v);
	}
}

// ---------------------------------------------------------------------------
// Expressions
// ---------------------------------------------------------------------------

void Visitor::Visit(ExprArrayLiteral *expr)
{
	VisitSelf(this, expr);

	for (auto elem : expr->elems)
	{
		AcceptChild(this, elem);
	}
}

void Visitor::Visit(ExprArraySlice *expr)
{
	VisitSelf(this, expr);
	AcceptChild(this, expr->left);
	AcceptChild(this, expr->start);
	AcceptChild(this, expr->end);
	AcceptChild(this, expr->step);
}

void Visitor::Visit(ExprBinaryOp *expr)
{
	VisitSelf(this, expr);
	AcceptChild(this, expr->left);
	AcceptChild(this, expr->right);
}

void Visitor::Visit(ExprCall *expr)
{
	VisitSelf(this, expr);
	AcceptChild(this, expr->left);

	for (auto arg : expr->args)
	{
		AcceptChild(this, arg);
	}
}

void Visitor::Visit(ExprFieldAccess *expr)
{
	VisitSelf(this, expr);
	AcceptChild(this, expr->left);
}

void Visitor::Visit(ExprFunDecl *expr)
{
	VisitSelf(this, expr);
	AcceptChild(this, expr->body);
}

void Visitor::Visit(ExprId *expr)
{
	VisitSelf(this, expr);
}

void Visitor::Visit(ExprIndexAccess *expr)
{
	VisitSelf(this, expr);
	AcceptChild(this, expr->left);
	AcceptChild(this, expr->index);
}

void Visitor::Visit(ExprLambda *expr)
{
	VisitSelf(this, expr);
	AcceptChild(this, expr->body);
}

void Visitor::Visit(ExprListComprehension *expr)
{
	VisitSelf(this, expr);
	AcceptChild(this, expr->expr);

	for (auto filter : expr->filters)
	{
		AcceptChild(this, filter);
	}

	for (auto generator : expr->generators)
	{
		AcceptChild(this, generator.from);
	}
}

void Visitor::Visit(ExprNumberLiteral *expr)
{
	VisitSelf(this, expr);
}

void Visitor::Visit(ExprObjectLiteral *expr)
{
	VisitSelf(this, expr);

	for (auto fn : expr->fnEntries)
	{
		AcceptChild(this, fn);
	}

	for (auto kv : expr->kvEntries)
	{
		AcceptChild(this, kv.value);
	}
}

void Visitor::Visit(ExprSimpleLiteral *expr)
{
	VisitSelf(this, expr);
}

void Visitor::Visit(ExprStringLiteral *expr)
{
	VisitSelf(this, expr);
}

void Visitor::Visit(ExprTernaryOp *expr)
{
	VisitSelf(this, expr);
	AcceptChild(this, expr->cond);
	AcceptChild(this, expr->thenExpr);
	AcceptChild(this, expr->elseExpr);
}

void Visitor::Visit(ExprUnaryOp *expr)
{
	VisitSelf(this, expr);
	AcceptChild(this, expr->value);
}

void Visitor::Visit(ExprYield *expr)
{
	VisitSelf(this, expr);
	AcceptChild(this, expr->value);
}

// ---------------------------------------------------------------------------
// Statements
// ---------------------------------------------------------------------------

void Visitor::Visit(StmtBlock *stmt)
{
	VisitSelf(this, stmt);

	for (auto sub : stmt->statements)
	{
		AcceptChild(this, sub);
	}
}

void Visitor::Visit(StmtControl *stmt)
{
	VisitSelf(this, stmt);
}

void Visitor::Visit(StmtDoWhile *stmt)
{
	VisitSelf(this, stmt);
	AcceptChild(this, stmt->body);
	AcceptChild(this, stmt->cond);
}

void Visitor::Visit(StmtForeach *stmt)
{
	VisitSelf(this, stmt);
	AcceptChild(this, stmt->from);
	AcceptChild(this, stmt->body);
}

void Visitor::Visit(StmtIfElse *stmt)
{
	VisitSelf(this, stmt);
	AcceptChild(this, stmt->cond);
	AcceptChild(this, stmt->thenBody);
	AcceptChild(this, stmt->elseBody);
}

void Visitor::Visit(StmtNakedExpr *stmt)
{
	VisitSelf(this, stmt);
	AcceptChild(this, stmt->value);
}

void Visitor::Visit(StmtReturn *stmt)
{
	VisitSelf(this, stmt);
	AcceptChild(this, stmt->value);
}

void Visitor::Visit(StmtSwitch *stmt)
{
	VisitSelf(this, stmt);

	for (auto subCase : stmt->cases)
	{
		AcceptChild(this, subCase.cond);

		for (auto subStmt : subCase.body)
		{
			AcceptChild(this, subStmt);
		}
	}
}

void Visitor::Visit(StmtVarDecl *stmt)
{
	VisitSelf(this, stmt);

	for (auto decl : stmt->decls)
	{
		AcceptChild(this, decl.value);
	}
}

void Visitor::Visit(StmtWhile *stmt)
{
	VisitSelf(this, stmt);
	AcceptChild(this, stmt->cond);
	AcceptChild(this, stmt->body);
}