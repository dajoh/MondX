#include "Parser.hpp"

using namespace Mond;

// ---------------------------------------------------------------------------
// Parser interface
// ---------------------------------------------------------------------------

Parser::Parser(DiagBuilder &diag, Source &source, Lexer &lexer, Sema &sema) :
	m_sema(sema),
	m_lexer(lexer),
	m_source(source),
	m_diag(diag)
{
	Advance();
}

StmtPtr Parser::ParseFile()
{
	auto stmt = new StmtBlock;
	stmt->pos = m_token.range.beg;
	stmt->range.beg = m_token.range.beg;

	while (m_token.type != TokEndOfFile)
	{
		stmt->statements.push_back(ParseStmt());
	}

	stmt->range.end = m_token.range.beg;
	return StmtPtr(stmt);
}

ExprPtr Parser::ParseExpr()
{
	return ExprPtr(ParseExprCore(Precedence::Invalid));
}

StmtPtr Parser::ParseStmt()
{
	return StmtPtr(ParseStmtCore());
}

// ---------------------------------------------------------------------------
// Core parser methods
// ---------------------------------------------------------------------------

void Parser::More()
{
	while (true)
	{
		auto token = m_lexer.GetToken();

		switch (token.type)
		{
		case TokEndOfLine:
		case TokUnknown:
		case TokCompletion:
		case TokWhiteSpace:
		case TokLineComment:
		case TokBlockComment:
			continue;
		default:
			m_lookahead.push_back(token);
			return;
		}
	}
}

void Parser::Advance()
{
	if (m_lookahead.empty())
	{
		More();
	}

	m_token = m_lookahead.front();
	m_lookahead.pop_front();
}

Token Parser::EatToken()
{
	auto current = m_token;
	Advance();
	return current;
}

Token Parser::EatToken(TokenType type)
{
	auto current = m_token;
	if (current.type == type)
	{
		Advance();
		return current;
	}

	return CreateMissing(type, true);
}

Token Parser::Lookahead(int n)
{
	while (n + 1 > (int)m_lookahead.size())
	{
		More();
	}

	return m_lookahead[n];
}

Token Parser::CreateMissing(TokenType type, bool error)
{
	if (error)
	{
		m_diag
			<< m_token.range
			<< Error
			<< ParseExpectedTokenGotOther
			<< type
			<< m_token.type
			<< DiagEnd;
	}

	Token newToken;
	newToken.type = type;
	newToken.range = Range(m_token.range.beg, 0);
	return newToken;
}

string Parser::IdString(Slice s)
{
	return m_source.GetSlice(s);
}

string Parser::LiteralString(Slice s)
{
	// TODO: Implement.
	return m_source.GetSlice(s);
}

double Parser::LiteralNumber(Slice s)
{
	// TODO: Yeah uh, this ain't gonna work. Handle 0x 0b and underscores.
	return strtod(m_source.GetSlice(s).c_str(), NULL);
}

// ---------------------------------------------------------------------------
// Expressions
// ---------------------------------------------------------------------------

bool Parser::CanBeExpr()
{
	switch (m_token.type)
	{
	case TokIdentifier:
	case TokStringLiteral:
	case TokNumberLiteral:
	case KwGlobal:
	case KwNull:
	case KwUndefined:
	case KwTrue:
	case KwFalse:
	case KwNaN:
	case KwInfinity:
	case TokLeftParen:
	case TokLeftBrace:
	case TokLeftBracket:
	case KwFun:
	case KwSeq:
	case KwYield:
		return true;
	default:
		return IsPrefixOperator(m_token.type);
	}
}

// TODO: Associativity.
ExprPtr Parser::ParseExprCore(Precedence p)
{
	ExprPtr left;

	switch (m_token.type)
	{
	case TokIdentifier:
		left = ParseExprId();
		break;
	case TokStringLiteral:
		left = ParseExprStringLiteral();
		break;
	case TokNumberLiteral:
		left = ParseExprNumberLiteral();
		break;
	case KwGlobal:
	case KwNull:
	case KwUndefined:
	case KwTrue:
	case KwFalse:
	case KwNaN:
	case KwInfinity:
		left = ParseExprSimpleLiteral();
		break;
	case TokLeftParen:
		left = ParseExprParens();
		break;
	case TokLeftBrace:
		left = ParseExprObjectLiteral();
		break;
	case TokLeftBracket:
		left = ParseExprArrayLiteral();
		break;
	case KwFun:
	case KwSeq:
		left = ParseExprLambda();
		break;
	case KwYield:
		left = ParseExprYield();
		break;
	default:
		if (!IsPrefixOperator(m_token.type))
		{
			m_diag
				<< m_token.range
				<< Error
				<< ParseExpectedExpr
				<< DiagEnd;
			return NULL;
		}
		left = ParseExprPrefixOp();
		break;
	}

	while (true)
	{
		if (IsBinaryOperator(m_token.type))
		{
			auto pc = GetOperatorPrecedence(m_token.type);
			if (pc <= p)
			{
				return left;
			}

			left = ParseExprBinaryOp(left, pc);
			continue;
		}
		else if (IsPostfixOperator(m_token.type))
		{
			left = ParseExprPostfixOp(left);
			continue;
		}

		switch (m_token.type)
		{
		case TokLeftParen:
			left = ParseExprCall(left);
			break;
		case TokLeftBracket:
			left = ParseExprIndexAccess(left);
			break;
		case OpDot:
			left = ParseExprFieldAccess(left);
			break;
		case OpQuestionMark:
			left = ParseExprTernaryOp(left);
			break;
		default:
			return left;
		}
	}

	throw logic_error("unreachable in ParseExprCore");
}

ExprPtr Parser::ParseExprId()
{
	if (Lookahead().type == OpPointy)
	{
		return ParseExprLambda();
	}

	auto expr = new ExprId;
	expr->pos = m_token.range.beg;
	expr->name = IdString(m_token.slice);
	expr->range = m_token.range;
	EatToken();

	m_sema.Visit(expr);
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprStringLiteral()
{
	auto expr = new ExprStringLiteral;
	expr->pos = m_token.range.beg;
	expr->range = m_token.range;
	expr->contents = LiteralString(m_token.slice);
	EatToken();

	m_sema.Visit(expr);
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprNumberLiteral()
{
	auto expr = new ExprNumberLiteral;
	expr->pos = m_token.range.beg;
	expr->range = m_token.range;
	expr->value = LiteralNumber(m_token.slice);
	EatToken();

	m_sema.Visit(expr);
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprSimpleLiteral()
{
	auto expr = new ExprSimpleLiteral;
	expr->pos = m_token.range.beg;
	expr->type = m_token.type;
	expr->range = m_token.range;
	EatToken();

	m_sema.Visit(expr);
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprParens()
{
	if (Lookahead().type == TokIdentifier)
	{
		if (Lookahead(1).type == TokComma || (Lookahead(1).type == TokRightParen && Lookahead(2).type == OpPointy))
		{
			return ParseExprLambda();
		}
	}
	else if (Lookahead().type == TokRightParen)
	{
		return ParseExprLambda();
	}

	auto beg = EatToken();
	auto expr = ParseExprCore(Precedence::Invalid);
	auto end = EatToken(TokRightParen);

	if (expr)
	{
		expr->range.beg = beg.range.beg;
		expr->range.end = end.range.end;
		expr->Accept(&m_sema);
	}

	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprObjectLiteral()
{
	auto expr = new ExprObjectLiteral;
	expr->pos = m_token.range.beg;
	expr->range = m_token.range;

	EatToken();

	while (m_token.type != TokRightBrace)
	{
		auto entry = ExprObjectLiteral::KeyValue();
		bool wantsExpr = false;

		if (m_token.type == TokIdentifier)
		{
			entry.key = IdString(EatToken().slice);
			wantsExpr = m_token.type == TokColon;
		}
		else if (m_token.type == TokStringLiteral)
		{
			entry.key = LiteralString(EatToken().slice);
			wantsExpr = true;
		}
		else
		{
			m_diag
				<< m_token.range.beg
				<< Error
				<< ParseExpectedObjectEntry
				<< DiagEnd;
			break;
		}

		if (wantsExpr)
		{
			auto colon = EatToken(TokColon);

			if ((m_token.type == TokIdentifier || m_token.type == TokStringLiteral) && Lookahead().type == TokColon)
			{
				// Special error handling for a common error while editing:
				// var x = {
				//   key1:
				//   key2: "banana"
				// };
				m_diag
					<< colon.range.end
					<< Error
					<< ParseExpectedExpr
					<< DiagEnd;
			}
			else
			{
				entry.value = ParseExpr();
			}
		}

		expr->entries.push_back(entry);

		if (m_token.type == TokComma)
		{
			EatToken();
		}
		else if (m_token.type == TokIdentifier || m_token.type == TokStringLiteral || m_token.type == KwFun || m_token.type == KwSeq)
		{
			// Forgotten comma after entry.
			EatToken(TokComma);
		}
		else
		{
			break;
		}
	}

	expr->range.end = ParseTerminator(TokRightBrace, expr->pos, ParseUnterminatedObjectLiteral);
	m_sema.Visit(expr);
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprArrayLiteral()
{
	auto expr = new ExprArrayLiteral;
	expr->pos = m_token.range.beg;
	expr->range = m_token.range;

	EatToken();

	while (m_token.type != TokRightBracket)
	{
		expr->elems.push_back(ParseExpr());

		if (m_token.type != TokComma)
		{
			break;
		}

		EatToken();
	}

	expr->range.end = ParseTerminator(TokRightBracket, expr->pos, ParseUnterminatedArrayLiteral);
	m_sema.Visit(expr);
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprYield()
{
	auto expr = new ExprYield;
	expr->pos = m_token.range.beg;
	expr->range = m_token.range;
	EatToken();

	if (CanBeExpr())
	{
		expr->value = ParseExpr();

		if (expr->value)
		{
			expr->range.end = expr->value->range.end;
		}
		else
		{
			expr->range.end = m_token.range.beg;
		}
	}

	m_sema.Visit(expr);
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprCall(ExprPtr left)
{
	auto expr = new ExprCall;
	expr->pos = m_token.range.beg;
	expr->left = left;
	expr->range = left->range;

	EatToken();

	if (m_token.type != TokRightParen)
	{
		while (true)
		{
			expr->args.push_back(ParseExpr());

			if (m_token.type == TokComma)
			{
				EatToken();
			}
			else
			{
				break;
			}
		}
	}

	expr->range.end = ParseTerminator(TokRightParen, expr->pos, ParseUnterminatedFunctionCall);
	m_sema.Visit(expr);
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprIndexAccess(ExprPtr left)
{
	auto expr = new ExprIndexAccess;
	expr->pos = m_token.range.beg;
	expr->left = left;
	expr->range = m_token.range;

	EatToken();

	if (m_token.type == TokColon)
	{
		auto pos = expr->pos;
		auto left = expr->left;
		delete expr;
		return ParseExprArraySlice(pos, left, NULL);
	}

	expr->index = ParseExpr();

	if (m_token.type == TokColon)
	{
		auto pos = expr->pos;
		auto left = expr->left;
		auto index = expr->index;
		delete expr;
		return ParseExprArraySlice(pos, left, index);
	}

	if (expr->left)
	{
		expr->range.beg = expr->left->range.beg;
	}

	expr->range.end = EatToken(TokRightBracket).range.end;
	m_sema.Visit(expr);
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprFieldAccess(ExprPtr left)
{
	auto expr = new ExprFieldAccess;
	expr->pos = m_token.range.beg;
	expr->left = left;
	expr->range = left->range;

	EatToken();

	auto member = EatToken(TokIdentifier);
	expr->name = IdString(member.slice);
	expr->range.end = member.range.end;
	m_sema.Visit(expr);
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprPrefixOp()
{
	auto expr = new ExprUnaryOp();
	expr->pos = m_token.range.beg;
	expr->type = m_token.type;
	expr->post = false;
	expr->range = m_token.range;

	EatToken();

	expr->value = ParseExpr();
	if (expr->value)
	{
		expr->range.end = expr->value->range.end;
	}
	else
	{
		expr->range.end = m_token.range.beg;
	}

	m_sema.Visit(expr);
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprPostfixOp(ExprPtr left)
{
	auto expr = new ExprUnaryOp;
	expr->type = m_token.type;
	expr->pos = m_token.range.beg;
	expr->post = true;
	expr->value = left;
	expr->range = m_token.range;

	EatToken();

	if (expr->value)
	{
		expr->range.beg = expr->value->range.beg;
	}

	m_sema.Visit(expr);
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprBinaryOp(ExprPtr left, Precedence p)
{
	auto expr = new ExprBinaryOp;
	expr->pos = m_token.range.beg;
	expr->type = m_token.type;
	expr->left = left;

	EatToken();

	expr->right = ParseExprCore(p);

	if (expr->right)
	{
		expr->range = Range(left->range.beg, expr->right->range.end);
	}
	else
	{
		expr->range = Range(left->range.beg, expr->pos);
	}

	m_sema.Visit(expr);
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprTernaryOp(ExprPtr left)
{
	auto expr = new ExprTernaryOp;
	expr->pos = m_token.range.beg;
	expr->cond = left;
	expr->range = m_token.range;

	EatToken();
	expr->thenExpr = ParseExpr();
	EatToken(TokColon);
	expr->elseExpr = ParseExpr();

	if (expr->cond)
	{
		expr->range.beg = expr->cond->range.beg;
	}

	if (expr->elseExpr)
	{
		expr->range.end = expr->elseExpr->range.end;
	}
	else
	{
		expr->range.end = m_token.range.beg;
	}

	m_sema.Visit(expr);
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprLambda()
{
	auto shortHand = m_token.type != KwFun && m_token.type != KwSeq;
	auto scopeType = m_token.type == KwSeq ? Scope::Sequence : Scope::Function;

	auto expr = new ExprLambda;
	auto eptr = ExprPtr(expr);
	expr->pos = m_token.range.beg;
	expr->range = m_token.range;
	expr->sequence = m_token.type == KwSeq;

	SemaScope scope(m_sema, scopeType, eptr);

	if (m_token.type == KwFun || m_token.type == KwSeq)
	{
		expr->sequence = m_token.type == KwSeq;

		EatToken();
		ParseArgumentList(expr->varargs);

		if (m_token.type != OpPointy)
		{
			expr->body = ParseStmtBlock();
			if (expr->body)
			{
				expr->range.end = expr->body->range.end;
			}
			else
			{
				expr->range.end = m_token.range.beg;
			}

			m_sema.Visit(expr);
			return ExprPtr(expr);
		}
	}
	else if (m_token.type == TokIdentifier)
	{
		auto arg = EatToken();
		m_sema.Declare(Decl::Argument, arg.range, IdString(arg.slice), eptr);
	}
	else
	{
		ParseArgumentList(expr->varargs);
	}

	expr->body = ParseStmtLambdaBody(shortHand);
	if (expr->body)
	{
		expr->range.end = expr->body->range.end;
	}
	else
	{
		expr->range.end = m_token.range.beg;
	}

	m_sema.Visit(expr);
	return eptr;
}

ExprPtr Parser::ParseExprCondition()
{
	EatToken(TokLeftParen);
	auto expr = ParseExprCore(Precedence::Invalid);
	EatToken(TokRightParen);

	if (expr)
	{
		expr->Accept(&m_sema);
	}

	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprArraySlice(Pos pos, ExprPtr left, ExprPtr first)
{
	auto expr = new ExprArraySlice;
	expr->pos = pos;
	expr->left = left;
	expr->start = first;

	if (expr->left)
	{
		expr->range.beg = expr->left->range.beg;
	}

	EatToken();

	if (m_token.type == TokRightBracket)
	{
		expr->range.end = EatToken().range.end;
		m_sema.Visit(expr);
		return ExprPtr(expr);
	}

	if (CanBeExpr())
	{
		expr->end = ParseExprCore(Precedence::Invalid);

		if (m_token.type == TokRightBracket)
		{
			expr->range.end = EatToken().range.end;
			m_sema.Visit(expr);
			return ExprPtr(expr);
		}
	}

	EatToken(TokColon);

	expr->step = ParseExprCore(Precedence::Invalid);
	expr->range.end = ParseTerminator(TokRightBracket, expr->pos, ParseUnterminatedArraySlice);
	m_sema.Visit(expr);
	return ExprPtr(expr);
}

// ---------------------------------------------------------------------------
// Statements
// ---------------------------------------------------------------------------

StmtPtr Parser::ParseStmtCore()
{
	switch (m_token.type)
	{
	case TokRightParen:
	case TokRightBrace:
	case TokRightBracket:
		m_diag
			<< m_token.range.beg
			<< Error
			<< ParseMismatchedToken
			<< m_token.type
			<< DiagEnd;
		EatToken();
		return NULL;
	default:
		break;
	}

	switch (m_token.type)
	{
	case TokSemicolon:
		EatToken();
		return NULL;
	case TokLeftBrace:
		return ParseStmtBlock();
	case KwBreak:
	case KwContinue:
		return ParseStmtControl();
	case KwDo:
		return ParseStmtDoWhile();
	case KwFor:
		return ParseStmtFor();
	case KwForeach:
		return ParseStmtForeach();
	case KwFun:
	case KwSeq:
		return ParseStmtFunDecl();
	case KwIf:
		return ParseStmtIfElse();
	case KwReturn:
		return ParseStmtReturn();
	case KwVar:
	case KwConst:
		return ParseStmtVarDecl();
	case KwSwitch:
		return ParseStmtSwitch();
	case KwWhile:
		return ParseStmtWhile();
	default:
		break;
	}

	if (CanBeExpr())
	{
		return ParseStmtNakedExpr();
	}

	m_diag
		<< m_token.range
		<< Error
		<< ParseExpectedStmt
		<< DiagEnd;
	EatToken();
	return NULL;
}

StmtPtr Parser::ParseStmtBlock()
{
	auto stmt = new StmtBlock;
	auto sptr = StmtPtr(stmt);
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;

	SemaScope scope(m_sema, Scope::Block, sptr);
	EatToken(TokLeftBrace);

	while (m_token.type != TokRightBrace && m_token.type != TokEndOfFile)
	{
		stmt->statements.push_back(ParseStmt());
	}

	stmt->range.end = EatToken(TokRightBrace).range.end;
	m_sema.Visit(stmt);
	return sptr;
}

StmtPtr Parser::ParseStmtControl()
{
	auto stmt = new StmtControl;
	stmt->pos = m_token.range.beg;
	stmt->type = m_token.type;
	stmt->range = m_token.range;

	EatToken();

	stmt->range.end = EatToken(TokSemicolon).range.end;
	m_sema.Visit(stmt);
	return StmtPtr(stmt);
}

StmtPtr Parser::ParseStmtDoWhile()
{
	auto stmt = new StmtDoWhile;
	auto sptr = StmtPtr(stmt);
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;

	SemaScope scope(m_sema, Scope::Loop, sptr);
	EatToken();
	stmt->body = ParseStmtCore();
	EatToken(KwWhile);
	stmt->cond = ParseExprCondition();

	stmt->range.end = EatToken(TokSemicolon).range.end;
	m_sema.Visit(stmt);
	return sptr;
}

StmtPtr Parser::ParseStmtFor()
{
	auto stmt = new StmtFor;
	auto sptr = StmtPtr(stmt);
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;

	SemaScope scope(m_sema, Scope::Loop, sptr);
	EatToken();

	EatToken(TokLeftParen);
	{
		if (m_token.type == KwVar || m_token.type == KwConst)
		{
			stmt->init = ParseStmtVarDecl();
		}
		else
		{
			if (CanBeExpr())
			{
				auto init = new StmtNakedExpr;

				init->value = ParseExpr();
				if (init->value)
				{
					init->pos = init->value->pos;
					init->range = init->value->range;
				}

				stmt->init.reset(init);
			}

			EatToken(TokSemicolon);
		}

		if (CanBeExpr())
		{
			stmt->cond = ParseExpr();
		}

		EatToken(TokSemicolon);

		if (CanBeExpr())
		{
			while (true)
			{
				stmt->steps.push_back(ParseExpr());

				if (m_token.type == TokComma)
				{
					EatToken();
					continue;
				}

				break;
			}
		}
	}
	EatToken(TokRightParen);

	stmt->body = ParseStmtCore();
	if (stmt->body)
	{
		stmt->range.end = stmt->body->range.end;
	}
	else
	{
		stmt->range.end = m_token.range.beg;
	}

	m_sema.Visit(stmt);
	return sptr;
}

StmtPtr Parser::ParseStmtForeach()
{
	auto stmt = new StmtForeach;
	auto sptr = StmtPtr(stmt);
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;

	SemaScope scope(m_sema, Scope::Loop, sptr);
	EatToken();

	EatToken(TokLeftParen);
	{
		EatToken(KwVar);
		auto id = EatToken(TokIdentifier);
		m_sema.Declare(Decl::Variable, id.range, IdString(id.slice), sptr);
		EatToken(KwIn);
		stmt->from = ParseExpr();
	}
	EatToken(TokRightParen);

	stmt->body = ParseStmtCore();
	if (stmt->body)
	{
		stmt->range.end = stmt->body->range.end;
	}
	else
	{
		stmt->range.end = m_token.range.beg;
	}

	m_sema.Visit(stmt);
	return sptr;
}

StmtPtr Parser::ParseStmtFunDecl()
{
	auto declType = m_token.type == KwFun ? Decl::Function : Decl::Sequence;
	auto scopeType = m_token.type == KwFun ? Scope::Function : Scope::Sequence;

	auto stmt = new StmtFunDecl;
	auto sptr = StmtPtr(stmt);
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;

	EatToken();

	auto id = EatToken(TokIdentifier);
	m_sema.Declare(declType, id.range, IdString(id.slice), sptr);

	SemaScope scope(m_sema, scopeType, sptr);
	ParseArgumentList(stmt->varargs);

	if (m_token.type == OpPointy)
	{
		stmt->body = ParseStmtLambdaBody(false);
		stmt->range.end = EatToken(TokSemicolon).range.end;
	}
	else
	{
		stmt->body = ParseStmtBlock();
		if (stmt->body)
		{
			stmt->range.end = stmt->body->range.end;
		}
		else
		{
			stmt->range.end = m_token.range.beg;
		}
	}

	m_sema.Visit(stmt);
	return sptr;
}

StmtPtr Parser::ParseStmtIfElse()
{
	auto stmt = new StmtIfElse;
	auto sptr = StmtPtr(stmt);
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;

	SemaScope scope(m_sema, Scope::Block, sptr);
	EatToken();

	stmt->cond = ParseExprCondition();
	stmt->thenBody = ParseStmtCore();

	if (m_token.type == KwElse)
	{
		EatToken();
		stmt->elseBody = ParseStmtCore();
	}

	if (stmt->elseBody)
	{
		stmt->range.end = stmt->elseBody->range.end;
	}
	else if (stmt->thenBody)
	{
		stmt->range.end = stmt->thenBody->range.end;
	}
	else
	{
		stmt->range.end = m_token.range.beg;
	}

	m_sema.Visit(stmt);
	return sptr;
}

StmtPtr Parser::ParseStmtReturn()
{
	auto stmt = new StmtReturn;
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;

	EatToken();

	if (CanBeExpr())
	{
		stmt->value = ParseExpr();
	}

	stmt->range.end = EatToken(TokSemicolon).range.end;
	m_sema.Visit(stmt);
	return StmtPtr(stmt);
}

StmtPtr Parser::ParseStmtVarDecl()
{
	auto type = m_token.type == KwVar ? Decl::Variable : Decl::Constant;
	auto stmt = new StmtVarDecl;
	auto sptr = StmtPtr(stmt);
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;

	EatToken();

	while (true)
	{
		auto id = EatToken(TokIdentifier);
		m_sema.Declare(type, id.range, IdString(id.slice), sptr);

		if (m_token.type == TokComma || m_token.type == TokSemicolon)
		{
			stmt->values.push_back(NULL);

			if (type == Decl::Constant)
			{
				m_diag
					<< id.range
					<< Error
					<< ParseConstNotInitialized
					<< DiagEnd;
			}

			if (m_token.type == TokComma)
			{
				EatToken();
				continue;
			}

			break;
		}

		EatToken(OpAssign);
		stmt->values.push_back(ParseExpr());

		if (m_token.type == TokComma)
		{
			EatToken();
			continue;
		}

		break;
	}

	stmt->range.end = EatToken(TokSemicolon).range.end;
	m_sema.Visit(stmt);
	return sptr;
}

StmtPtr Parser::ParseStmtSwitch()
{
	auto stmt = new StmtSwitch;
	auto sptr = StmtPtr(stmt);
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;

	SemaScope scope(m_sema, Scope::Block, sptr);
	EatToken();
	stmt->value = ParseExprCondition();
	EatToken(TokLeftBrace);

	while (m_token.type != TokRightBrace && m_token.type != TokEndOfFile)
	{
		auto current = StmtSwitch::Case();

		current.headRange.beg = m_token.range.beg;

		if (m_token.type == KwCase)
		{
			EatToken();
			current.def = false;
			current.value = ParseExpr();
			current.headRange.end = EatToken(TokColon).range.end;
		}
		else if (m_token.type == KwDefault)
		{
			EatToken();
			current.def = true;
			current.headRange.end = EatToken(TokColon).range.end;
		}
		else
		{
			m_diag
				<< m_token.range.beg
				<< Error
				<< ParseExpectedSwitchCase
				<< DiagEnd;
		}

		while (true)
		{
			switch (m_token.type)
			{
			case TokEndOfFile:
			case TokRightBrace:
			case KwCase:
			case KwDefault:
				break;
			default:
				current.body.push_back(ParseStmt());
				continue;
			}

			break;
		}

		stmt->cases.push_back(current);
	}

	stmt->range.end = EatToken(TokRightBrace).range.end;
	m_sema.Visit(stmt);
	return sptr;
}

StmtPtr Parser::ParseStmtWhile()
{
	auto stmt = new StmtWhile;
	auto sptr = StmtPtr(stmt);
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;

	SemaScope scope(m_sema, Scope::Loop, sptr);
	EatToken();

	stmt->cond = ParseExprCondition();
	stmt->body = ParseStmtCore();

	if (stmt->body)
	{
		stmt->range.end = stmt->body->range.end;
	}
	else
	{
		stmt->range.end = m_token.range.beg;
	}

	m_sema.Visit(stmt);
	return sptr;
}

StmtPtr Parser::ParseStmtNakedExpr()
{
	auto stmt = new StmtNakedExpr;
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;
	stmt->value = ParseExprCore(Precedence::Invalid);
	stmt->range.end = EatToken(TokSemicolon).range.end;
	m_sema.Visit(stmt);
	return StmtPtr(stmt);
}

StmtPtr  Mond::Parser::ParseStmtLambdaBody(bool isShorthand)
{
	auto pointy = EatToken(OpPointy);

	if (m_token.type == TokLeftBrace)
	{
		if (!isShorthand)
		{
			m_diag
				<< pointy.range
				<< Info
				<< ParseUnnecessaryPointyInFun
				<< DiagEnd;
		}

		return ParseStmtBlock();
	}

	auto stmt = new StmtReturn;
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;

	stmt->value = ParseExprCore(Precedence::Invalid);
	if (stmt->value)
	{
		stmt->range.end = stmt->value->range.end;
	}
	else
	{
		stmt->range.end = m_token.range.beg;
	}

	m_sema.Visit(stmt);
	return StmtPtr(stmt);
}

// ---------------------------------------------------------------------------
// Reused parsers
// ---------------------------------------------------------------------------

Pos Parser::ParseTerminator(TokenType type, Pos beg, DiagMessage msg)
{
	Token token;

	if (m_token.type != type)
	{
		token = CreateMissing(type, false);
		m_diag
			<< Range(beg, token.range.beg)
			<< Error
			<< msg
			<< DiagEnd;
	}
	else
	{
		token = EatToken();
	}

	return token.range.end;
}

void Parser::ParseArgumentList(bool &varargs)
{
	varargs = false;

	EatToken(TokLeftParen);
	{
		if (m_token.type != TokRightParen)
		{
			while (true)
			{
				if (m_token.type == OpEllipsis)
				{
					EatToken();
					varargs = true;
				}

				// TODO: AstNode
				auto id = EatToken(TokIdentifier);
				m_sema.Declare(Decl::Argument, id.range, IdString(id.slice), NULL);

				if (!varargs && m_token.type == TokComma)
				{
					EatToken();
				}
				else
				{
					break;
				}
			}
		}
	}
	EatToken(TokRightParen);
}
