#include "AST.hpp"

using namespace Mond;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

template<class T>
void VisitSelf(Visitor *v, T t)
{
	v->Visit((AstNode *)t);
}

void Mond::AcceptChild(Visitor *v, AstNode *n)
{
	if (n)
	{
		n->Accept(v);
	}
}

// ---------------------------------------------------------------------------
// AST Node
// ---------------------------------------------------------------------------

void Visitor::Visit(AstNode *node)
{
}

// ---------------------------------------------------------------------------
// Expressions
// ---------------------------------------------------------------------------

void Visitor::Visit(Expr *expr)
{
}

void Visitor::Visit(ExprArrayLiteral *expr)
{
	VisitSelf(this, expr);

	for (auto &elem : expr->elems)
	{
		AcceptChild(this, elem.get());
	}
}

void Visitor::Visit(ExprArraySlice *expr)
{
	VisitSelf(this, expr);
	AcceptChild(this, expr->left.get());
	AcceptChild(this, expr->start.get());
	AcceptChild(this, expr->end.get());
	AcceptChild(this, expr->step.get());
}

void Visitor::Visit(ExprBinaryOp *expr)
{
	VisitSelf(this, expr);
	AcceptChild(this, expr->left.get());
	AcceptChild(this, expr->right.get());
}

void Visitor::Visit(ExprCall *expr)
{
	VisitSelf(this, expr);
	AcceptChild(this, expr->left.get());

	for (auto &arg : expr->args)
	{
		AcceptChild(this, arg.get());
	}
}

void Visitor::Visit(ExprFieldAccess *expr)
{
	VisitSelf(this, expr);
	AcceptChild(this, expr->left.get());
}

void Visitor::Visit(ExprId *expr)
{
	VisitSelf(this, expr);
}

void Visitor::Visit(ExprIndexAccess *expr)
{
	VisitSelf(this, expr);
	AcceptChild(this, expr->left.get());
	AcceptChild(this, expr->index.get());
}

void Visitor::Visit(ExprLambda *expr)
{
	VisitSelf(this, expr);
	AcceptChild(this, expr->body.get());
}

void Visitor::Visit(ExprNumberLiteral *expr)
{
	VisitSelf(this, expr);
}

void Visitor::Visit(ExprObjectLiteral *expr)
{
	VisitSelf(this, expr);

	for (auto &entry : expr->entries)
	{
		AcceptChild(this, entry.value.get());
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
	AcceptChild(this, expr->cond.get());
	AcceptChild(this, expr->thenExpr.get());
	AcceptChild(this, expr->elseExpr.get());
}

void Visitor::Visit(ExprUnaryOp *expr)
{
	VisitSelf(this, expr);
	AcceptChild(this, expr->value.get());
}

void Visitor::Visit(ExprYield *expr)
{
	VisitSelf(this, expr);
	AcceptChild(this, expr->value.get());
}

// ---------------------------------------------------------------------------
// Statements
// ---------------------------------------------------------------------------

void Visitor::Visit(Stmt *stmt)
{
}

void Visitor::Visit(StmtBlock *stmt)
{
	VisitSelf(this, stmt);

	for (auto &sub : stmt->statements)
	{
		AcceptChild(this, sub.get());
	}
}

void Visitor::Visit(StmtControl *stmt)
{
	VisitSelf(this, stmt);
}

void Visitor::Visit(StmtDoWhile *stmt)
{
	VisitSelf(this, stmt);
	AcceptChild(this, stmt->body.get());
	AcceptChild(this, stmt->cond.get());
}

void Visitor::Visit(StmtFor *stmt)
{
	VisitSelf(this, stmt);
	AcceptChild(this, stmt->init.get());
	AcceptChild(this, stmt->cond.get());

	for (auto &step : stmt->steps)
	{
		AcceptChild(this, step.get());
	}

	AcceptChild(this, stmt->body.get());
}

void Visitor::Visit(StmtForeach *stmt)
{
	VisitSelf(this, stmt);
	AcceptChild(this, stmt->from.get());
	AcceptChild(this, stmt->body.get());
}

void Visitor::Visit(StmtFunDecl *expr)
{
	VisitSelf(this, expr);
	AcceptChild(this, expr->body.get());
}

void Visitor::Visit(StmtIfElse *stmt)
{
	VisitSelf(this, stmt);
	AcceptChild(this, stmt->cond.get());
	AcceptChild(this, stmt->thenBody.get());
	AcceptChild(this, stmt->elseBody.get());
}

void Visitor::Visit(StmtNakedExpr *stmt)
{
	VisitSelf(this, stmt);
	AcceptChild(this, stmt->value.get());
}

void Visitor::Visit(StmtReturn *stmt)
{
	VisitSelf(this, stmt);
	AcceptChild(this, stmt->value.get());
}

void Visitor::Visit(StmtSwitch *stmt)
{
	VisitSelf(this, stmt);

	for (auto &subCase : stmt->cases)
	{
		AcceptChild(this, subCase.value.get());

		for (auto &subStmt : subCase.body)
		{
			AcceptChild(this, subStmt.get());
		}
	}
}

void Visitor::Visit(StmtVarDecl *stmt)
{
	VisitSelf(this, stmt);

	for (auto &value : stmt->values)
	{
		AcceptChild(this, value.get());
	}
}

void Visitor::Visit(StmtWhile *stmt)
{
	VisitSelf(this, stmt);
	AcceptChild(this, stmt->cond.get());
	AcceptChild(this, stmt->body.get());
}