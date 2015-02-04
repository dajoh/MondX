#include "Source.hpp"

using namespace Mond;

StringSource::StringSource(const char *str) : m_beg(str), m_ptr(str)
{
}

// TODO: I probably suck, fix me.
string StringSource::GetLine(int line) const
{
	auto rpos = 0;
	auto cline = 1;
	stringstream buffer;

	while (true)
	{
		auto ch = m_beg[rpos];
		auto pk = ch ? m_beg[rpos + 1] : ch;

		if (ch == '\0')
		{
			break;
		}
		else if (ch == '\n')
		{
			cline++;
		}
		else if (ch == '\r' && pk != '\n')
		{
			cline++;
		}
		else if (cline > line)
		{
			break;
		}
		else if (cline == line)
		{
			buffer << ch;
		}

		rpos++;
	}

	return buffer.str();
}

string StringSource::GetSlice(Slice s) const
{
	return string(&m_beg[s.beg], s.end - s.beg);
}

// TODO: I probably suck, fix me.
string StringSource::GetRange(Range r) const
{
	auto rpos = 0;

	Pos pos(1, 1);
	Slice slice;

	while (true)
	{
		auto ch = m_beg[rpos];
		auto pk = ch ? m_beg[rpos + 1] : ch;

		if (pos.line == r.beg.line && pos.column == r.beg.column)
		{
			slice.beg = rpos;
		}

		if (pos.line == r.end.line && pos.column == r.end.column)
		{
			slice.end = rpos;
			break;
		}

		if (ch == '\0')
		{
			throw logic_error("range larger than file!");
		}

		if (ch == '\n')
		{
			pos.line++;
			pos.column = 1;
		}
		else if (ch == '\r' && pk != '\n')
		{
			pos.line++;
			pos.column = 1;
		}
		else
		{
			pos.column++;
		}

		rpos++;
	}

	return GetSlice(slice);
}

void StringSource::Advance()
{
	if (m_ptr[0] == '\0')
	{
		return;
	}

	m_ptr++;
}

int StringSource::Position() const
{
	return m_ptr - m_beg;
}

uint32_t StringSource::Cur() const
{
	return m_ptr[0];
}

uint32_t StringSource::Peek() const
{
	return m_ptr[0] == '\0' ? m_ptr[0] : m_ptr[1];
}