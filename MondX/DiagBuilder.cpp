#include "DiagBuilder.hpp"

using namespace Mond;

DiagBuilder::DiagBuilder(DiagObserver fn) :
	m_pos(0),
	m_fmt(nullptr),
	m_func(fn)
{
}

DiagBuilder &DiagBuilder::operator<<(Pos caret)
{
	m_diag.caret = caret;
	return *this;
}

DiagBuilder &DiagBuilder::operator<<(Range range)
{
	m_diag.range = range;
	return *this;
}

DiagBuilder &DiagBuilder::operator<<(Severity s)
{
	m_diag.severity = s;
	return *this;
}

DiagBuilder &DiagBuilder::operator<<(DiagMessage m)
{
	m_pos = 0;
	m_fmt = GetDiagMessageFormat(m);
	m_diag.messageId = m;
	return *this;
}

DiagBuilder &DiagBuilder::operator<<(uint32_t c)
{
	WriteUntilFormatter('c');

	if (isprint(c))
	{
		m_msg << (char)c;
	}
	else
	{
		m_msg << "<0x" << hex << c << ">";
	}

	return *this;
}

DiagBuilder &DiagBuilder::operator<<(TokenType t)
{
	WriteUntilFormatter('t');
	m_msg << GetTokenTypeName(t);
	return *this;
}

DiagBuilder &DiagBuilder::operator<<(const char *s)
{
	WriteUntilFormatter('s');
	m_msg << s;
	return *this;
}

DiagBuilder &DiagBuilder::operator<<(DiagSentinel b)
{
	WriteRest();

	m_diag.message = m_msg.str();
	m_func(m_diag);

	m_pos = 0;
	m_fmt = nullptr;
	m_msg.str("");
	m_diag = Diag();

	return *this;
}

void DiagBuilder::WriteRest()
{
	m_msg << &m_fmt[m_pos];
}

void DiagBuilder::WriteUntilFormatter(char expect)
{
	auto beg = m_pos;

	while (m_fmt[m_pos++] != '%')
	{
	}

	m_msg.write(&m_fmt[beg], m_pos - beg - 1);

	if (m_fmt[m_pos++] != expect)
	{
		throw logic_error("invalid diagnostic format string");
	}
}