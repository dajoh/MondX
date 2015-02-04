#include "Parser.hpp"

using namespace Mond;

// ---------------------------------------------------------------------------
// Parser interface
// ---------------------------------------------------------------------------

Parser::Parser(DiagBuilder &diag, Source &source, Lexer &lexer) :
	m_diag(diag),
	m_lexer(lexer),
	m_source(source)
{
	Advance();
}

StmtPtrList Parser::ParseFile()
{
	StmtPtrList stmts;

	while (m_token.type != TokEndOfFile)
	{
		stmts.push_back(ParseStmt());
	}

	return stmts;
}

ExprPtr Parser::ParseExpr()
{
	return ParseExprCore(Precedence::Invalid);
}

StmtPtr Parser::ParseStmt()
{
	return ParseStmtCore();
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
		case TokUnknown:
		case TokCompletion:
		case TokWhiteSpace:
		case TokLineComment:
		case TokBlockComment:
			continue;
		}

		m_lookahead.push_back(token);
		break;
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
			<< m_token.range.beg
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
	return strtod(m_source.GetSlice(s).c_str(), nullptr);
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
	}

	return IsPrefixOperator();
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
		left = ParseExprFunDecl();
		break;
	case KwYield:
		left = ParseExprYield();
		break;
	default:
		if (!IsPrefixOperator())
		{
			m_diag
				<< m_token.range.beg
				<< Error
				<< ParseExpectedExpr
				<< DiagEnd;
			return nullptr;
		}
		left = ParseExprPrefixOp();
		break;
	}

	while (true)
	{
		if (IsBinaryOperator())
		{
			auto pc = GetOperatorPrecedence();
			if (pc <= p)
			{
				return left;
			}

			left = ParseExprBinaryOp(left, pc);
			continue;
		}
		else if (IsPostfixOperator())
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
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprStringLiteral()
{
	auto expr = new ExprStringLiteral;
	expr->pos = m_token.range.beg;
	expr->range = m_token.range;
	expr->contents = LiteralString(m_token.slice);
	EatToken();
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprNumberLiteral()
{
	auto expr = new ExprNumberLiteral;
	expr->pos = m_token.range.beg;
	expr->range = m_token.range;
	expr->value = LiteralNumber(m_token.slice);
	EatToken();
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprSimpleLiteral()
{
	auto expr = new ExprSimpleLiteral;
	expr->pos = m_token.range.beg;
	expr->type = m_token.type;
	expr->range = m_token.range;
	EatToken();
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprParens()
{
	if (Lookahead().type == TokIdentifier)
	{
		if (Lookahead(1).type == TokComma || Lookahead(1).type == TokRightParen && Lookahead(2).type == OpPointy)
		{
			return ParseExprLambda();
		}
	}
	else if (Lookahead().type == TokRightParen)
	{
		return ParseExprLambda();
	}

	auto beg = EatToken();
	auto expr = ParseExpr();
	auto end = EatToken(TokRightParen);

	if (expr)
	{
		expr->range.beg = beg.range.beg;
		expr->range.end = end.range.end;
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
		if (m_token.type == KwFun || m_token.type == KwSeq)
		{
			expr->fnEntries.push_back(ParseExprFunDecl());
		}
		else
		{
			bool wantsExpr = false;
			ExprObjectLiteral::KeyValue entry;

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

			expr->kvEntries.push_back(entry);
		}

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

		if (m_token.type == TokColon && expr->elems.size() == 1)
		{
			auto pos = expr->pos;
			auto elem = expr->elems[0];
			delete expr;
			return ParseExprListComprehension(pos, elem);
		}
		else if (m_token.type != TokComma)
		{
			break;
		}

		EatToken();
	}

	expr->range.end = ParseTerminator(TokRightBracket, expr->pos, ParseUnterminatedArrayLiteral);
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprFunDecl()
{
	auto expr = new ExprFunDecl;
	expr->fun = m_token.type == KwFun;
	expr->pos = m_token.range.beg;
	expr->range = m_token.range;

	EatToken();

	if (m_token.type == TokIdentifier)
	{
		expr->name = IdString(EatToken().slice);
	}

	expr->args = ParseArgumentList(expr->varargs);

	if (m_token.type == OpPointy)
	{
		auto pointy = EatToken();

		if (m_token.type == TokLeftBrace)
		{
			m_diag
				<< pointy.range
				<< Info
				<< ParseUnnecessaryPointyInFun
				<< DiagEnd;
		}

		expr->body = ParseStmtLambdaBody();
		expr->semi = true;
	}
	else
	{
		expr->body = ParseStmtBlock();
		expr->semi = false;
	}

	if (expr->body)
	{
		expr->range.end = expr->body->range.end;
	}
	else
	{
		expr->range.end = m_token.range.beg;
	}

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
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprIndexAccess(ExprPtr left)
{
	auto expr = new ExprIndexAccess;
	expr->pos = m_token.range.beg;
	expr->left = left;
	expr->range = m_token.range;

	if (expr->left)
	{
		expr->range.beg = expr->left->range.beg;
	}

	EatToken();

	expr->index = ParseExpr();
	expr->range.end = EatToken(TokRightBracket).range.end;
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
	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprPrefixOp()
{
	auto expr = new ExprUnaryOp();
	expr->op = m_token.type;
	expr->pos = m_token.range.beg;
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

	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprPostfixOp(ExprPtr left)
{
	auto expr = new ExprUnaryOp;
	expr->op = m_token.type;
	expr->pos = m_token.range.beg;
	expr->post = true;
	expr->value = left;
	expr->range = m_token.range;

	EatToken();

	if (expr->value)
	{
		expr->range.beg = expr->value->range.beg;
	}

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

	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprLambda()
{
	auto expr = new ExprLambda;
	expr->pos = m_token.range.beg;
	expr->range = m_token.range;
	
	if (m_token.type == TokIdentifier)
	{
		expr->args.push_back(IdString(EatToken().slice));
	}
	else
	{
		expr->args = ParseArgumentList(expr->varargs);
	}

	EatToken(OpPointy);

	expr->body = ParseStmtLambdaBody();
	if (expr->body)
	{
		expr->range.end = expr->body->range.end;
	}
	else
	{
		expr->range.end = m_token.range.beg;
	}

	return ExprPtr(expr);
}

ExprPtr Parser::ParseExprCondition()
{
	EatToken(TokLeftParen);
	auto expr = ParseExpr();
	EatToken(TokRightParen);
	return expr;
}

ExprPtr Parser::ParseExprArraySlice(Pos pos, ExprPtr left, ExprPtr first)
{
	throw logic_error("not implemented");
}

// TODO: Mostly untested, works on valid input.
ExprPtr Parser::ParseExprListComprehension(Pos pos, ExprPtr first)
{
	auto expr = new ExprListComprehension;
	expr->pos = pos;
	expr->expr = first;
	expr->range.beg = pos;

	// Eat colon.
	EatToken();

	while (true)
	{
		if (m_token.type == TokIdentifier && Lookahead().type == KwIn)
		{
			ExprListComprehension::Generator gen;

			gen.name = IdString(EatToken(TokIdentifier).slice);
			EatToken(KwIn);
			gen.from = ParseExpr();

			expr->generators.push_back(gen);
		}
		else
		{
			expr->filters.push_back(ParseExpr());
		}

		if (m_token.type == TokComma)
		{
			EatToken();
		}
		else
		{
			break;
		}
	}

	expr->range.end = ParseTerminator(TokRightBracket, expr->pos, ParseUnterminatedListComprehension);
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
		return nullptr;
	}

	switch (m_token.type)
	{
	case TokSemicolon:
		EatToken();
		return nullptr;
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
	}

	if (CanBeExpr())
	{
		return ParseStmtNakedExpr();
	}

	m_diag
		<< m_token.range.beg
		<< Error
		<< ParseExpectedStmt
		<< DiagEnd;
	EatToken();
	return nullptr;
}

StmtPtr Parser::ParseStmtBlock()
{
	auto stmt = new StmtBlock;
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;

	EatToken(TokLeftBrace);

	while (m_token.type != TokRightBrace && m_token.type != TokEndOfFile)
	{
		stmt->statements.push_back(ParseStmt());
	}

	stmt->range.end = EatToken(TokRightBrace).range.end;
	return StmtPtr(stmt);
}

StmtPtr Parser::ParseStmtControl()
{
	auto stmt = new StmtControl;
	stmt->pos = m_token.range.beg;
	stmt->type = m_token.type;
	stmt->range = m_token.range;

	EatToken();

	stmt->range.end = EatToken(TokSemicolon).range.end;
	return StmtPtr(stmt);
}

StmtPtr Parser::ParseStmtDoWhile()
{
	auto stmt = new StmtDoWhile;
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;

	EatToken();
	stmt->body = ParseStmt();
	EatToken(KwWhile);
	stmt->cond = ParseExprCondition();

	auto semi = EatToken(TokSemicolon);
	stmt->range.end = semi.range.end;
	return StmtPtr(stmt);
}

// TODO: Mostly untested, works on valid input.
StmtPtr Parser::ParseStmtFor()
{
	auto stmt = new StmtFor;
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;

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

				stmt->init = StmtPtr(init);
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

	stmt->body = ParseStmt();
	if (stmt->body)
	{
		stmt->range.end = stmt->body->range.end;
	}
	else
	{
		stmt->range.end = m_token.range.beg;
	}

	return StmtPtr(stmt);
}

StmtPtr Parser::ParseStmtForeach()
{
	auto stmt = new StmtForeach;
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;

	EatToken();

	EatToken(TokLeftParen);
	{
		EatToken(KwVar);
		stmt->name = EatToken(TokIdentifier).slice;
		EatToken(KwIn);
		stmt->from = ParseExpr();
	}
	EatToken(TokRightParen);

	stmt->body = ParseStmt();
	if (stmt->body)
	{
		stmt->range.end = stmt->body->range.end;
	}
	else
	{
		stmt->range.end = m_token.range.beg;
	}

	return StmtPtr(stmt);
}

StmtPtr Parser::ParseStmtIfElse()
{
	auto stmt = new StmtIfElse;
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;

	EatToken();

	stmt->cond = ParseExprCondition();
	stmt->thenBody = ParseStmt();

	if (m_token.type == KwElse)
	{
		EatToken();
		stmt->elseBody = ParseStmt();
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

	return StmtPtr(stmt);
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

	auto semi = EatToken(TokSemicolon);
	stmt->range.end = semi.range.end;
	return StmtPtr(stmt);
}

StmtPtr Parser::ParseStmtVarDecl()
{
	auto stmt = new StmtVarDecl;
	stmt->var = m_token.type == KwVar;
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;

	EatToken();

	while (true)
	{
		auto id = EatToken(TokIdentifier);

		StmtVarDecl::Decl decl;
		decl.name = IdString(id.slice);

		if (m_token.type == TokComma || m_token.type == TokSemicolon)
		{
			stmt->decls.push_back(decl);

			if (!stmt->var)
			{
				m_diag
					<< id.range.beg
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
		decl.value = ParseExpr();
		stmt->decls.push_back(decl);

		if (m_token.type == TokComma)
		{
			EatToken();
			continue;
		}

		break;
	}

	auto semi = EatToken(TokSemicolon);
	stmt->range.end = semi.range.end;
	return StmtPtr(stmt);
}

// TODO: Mostly untested, works on valid input.
StmtPtr Parser::ParseStmtSwitch()
{
	auto stmt = new StmtSwitch;
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;

	EatToken();
	stmt->value = ParseExprCondition();
	EatToken(TokLeftBrace);

	while (m_token.type != TokRightBrace && m_token.type != TokEndOfFile)
	{
		StmtSwitch::Case current;

		if (m_token.type == KwCase)
		{
			EatToken();
			current.def = false;
			current.cond = ParseExpr();
		}
		else if (m_token.type == KwDefault)
		{
			EatToken();
			current.def = true;
		}
		else
		{
			throw logic_error("not implemented gosh");
		}

		EatToken(TokColon);

		while (true)
		{
			// this is silly...
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
	}

	stmt->range.end = EatToken(TokRightBrace).range.end;
	return StmtPtr(stmt);
}

StmtPtr Parser::ParseStmtWhile()
{
	auto stmt = new StmtWhile;
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;

	EatToken();

	stmt->cond = ParseExprCondition();
	stmt->body = ParseStmt();

	if (stmt->body)
	{
		stmt->range.end = stmt->body->range.end;
	}
	else
	{
		stmt->range.end = m_token.range.beg;
	}

	return StmtPtr(stmt);
}

StmtPtr Parser::ParseStmtNakedExpr()
{
	auto stmt = new StmtNakedExpr;
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;
	stmt->value = ParseExpr();

	if (stmt->value && !stmt->value->WantsSemi())
	{
		stmt->range.end = stmt->value->range.end;
	}
	else
	{
		stmt->range.end = EatToken(TokSemicolon).range.end;
	}

	return StmtPtr(stmt);
}

StmtPtr Parser::ParseStmtLambdaBody()
{
	if (m_token.type == TokLeftBrace)
	{
		return ParseStmtBlock();
	}

	auto stmt = new StmtReturn;
	stmt->pos = m_token.range.beg;
	stmt->range = m_token.range;

	stmt->value = ParseExpr();
	if (stmt->value)
	{
		stmt->range.end = stmt->value->range.end;
	}
	else
	{
		stmt->range.end = m_token.range.beg;
	}

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

vector<string> Parser::ParseArgumentList(bool &varargs)
{
	vector<string> args;

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

				args.push_back(IdString(EatToken(TokIdentifier).slice));

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

	return args;
}

// ---------------------------------------------------------------------------
// Operators
// ---------------------------------------------------------------------------

bool Parser::IsPrefixOperator() const
{
	switch (m_token.type)
	{
	case OpSubtract:
	case OpIncrement:
	case OpDecrement:
	case OpBitNot:
	case OpNot:
	case OpEllipsis:
		return true;
	}

	return false;
}

bool Parser::IsBinaryOperator() const
{
	switch (m_token.type)
	{
	case OpAdd:
	case OpSubtract:
	case OpMultiply:
	case OpDivide:
	case OpModulo:
	case OpExponent:
	case OpBitLeftShift:
	case OpBitRightShift:
	case OpBitAnd:
	case OpBitOr:
	case OpBitXor:

	case OpAssign:
	case OpAddAssign:
	case OpSubtractAssign:
	case OpMultiplyAssign:
	case OpDivideAssign:
	case OpModuloAssign:
	case OpExponentAssign:
	case OpBitLeftShiftAssign:
	case OpBitRightShiftAssign:
	case OpBitAndAssign:
	case OpBitOrAssign:
	case OpBitXorAssign:

	case OpEqualTo:
	case OpNotEqualTo:
	case OpGreaterThan:
	case OpGreaterThanOrEqual:
	case OpLessThan:
	case OpLessThanOrEqual:
	case OpConditionalAnd:
	case OpConditionalOr:
	case KwIn:
	case KwNotIn:

	case OpPipeline:
		return true;
	}

	return false;
}

bool Parser::IsPostfixOperator() const
{
	switch (m_token.type)
	{
	case OpIncrement:
	case OpDecrement:
		return true;
	}

	return false;
}

Precedence Parser::GetOperatorPrecedence()
{
	switch (m_token.type)
	{
	case OpAssign:
	case OpAddAssign:
	case OpSubtractAssign:
	case OpMultiplyAssign:
	case OpDivideAssign:
	case OpModuloAssign:
	case OpExponentAssign:
	case OpBitLeftShiftAssign:
	case OpBitRightShiftAssign:
	case OpBitAndAssign:
	case OpBitOrAssign:
	case OpBitXorAssign:
		return Precedence::Assign;

	case OpQuestionMark:
		return Precedence::Ternary;

	case OpConditionalOr:
		return Precedence::ConditionalOr;

	case OpConditionalAnd:
		return Precedence::ConditionalAnd;

	case OpEqualTo:
	case OpNotEqualTo:
		return Precedence::Equality;

	case OpGreaterThan:
	case OpGreaterThanOrEqual:
	case OpLessThan:
	case OpLessThanOrEqual:
	case KwIn:
	case KwNotIn:
		return Precedence::Relational;

	case OpBitOr:
		return Precedence::BitOr;
	case OpBitXor:
		return Precedence::BitXor;
	case OpBitAnd:
		return Precedence::BitAnd;

	case OpBitLeftShift:
	case OpBitRightShift:
		return Precedence::BitShift;

	case OpAdd:
	case OpSubtract:
		return Precedence::Addition;

	case OpMultiply:
	case OpDivide:
	case OpModulo:
	case OpExponent:
		return Precedence::Multiplication;

	case OpPipeline:
		return Precedence::Misc;
	}

	throw logic_error("precedence requested for unknown operator");
}
