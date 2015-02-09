#ifndef MOND_VISITOR_HPP
#define MOND_VISITOR_HPP

namespace Mond
{
	class Visitor
	{
	public:
		virtual ~Visitor() {}

		virtual void Visit(struct AstNode *);

		virtual void Visit(struct Expr *);
		virtual void Visit(struct ExprArrayLiteral *);
		virtual void Visit(struct ExprArraySlice *);
		virtual void Visit(struct ExprBinaryOp *);
		virtual void Visit(struct ExprCall *);
		virtual void Visit(struct ExprFieldAccess *);
		virtual void Visit(struct ExprId *);
		virtual void Visit(struct ExprIndexAccess *);
		virtual void Visit(struct ExprLambda *);
		virtual void Visit(struct ExprNumberLiteral *);
		virtual void Visit(struct ExprObjectLiteral *);
		virtual void Visit(struct ExprSimpleLiteral *);
		virtual void Visit(struct ExprStringLiteral *);
		virtual void Visit(struct ExprTernaryOp *);
		virtual void Visit(struct ExprUnaryOp *);
		virtual void Visit(struct ExprYield *);

		virtual void Visit(struct Stmt *);
		virtual void Visit(struct StmtBlock *);
		virtual void Visit(struct StmtControl *);
		virtual void Visit(struct StmtDoWhile *);
		virtual void Visit(struct StmtFor *);
		virtual void Visit(struct StmtForeach *);
		virtual void Visit(struct StmtFunDecl *);
		virtual void Visit(struct StmtIfElse *);
		virtual void Visit(struct StmtNakedExpr *);
		virtual void Visit(struct StmtReturn *);
		virtual void Visit(struct StmtSwitch *);
		virtual void Visit(struct StmtVarDecl *);
		virtual void Visit(struct StmtWhile *);
	};

	void AcceptChild(Visitor *v, struct AstNode *n);
}

#endif
