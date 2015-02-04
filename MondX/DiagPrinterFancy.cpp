#include "DiagPrinterFancy.hpp"

using namespace Mond;

DiagPrinterFancy::DiagPrinterFancy(Source &source) : m_source(source)
{
	CONSOLE_SCREEN_BUFFER_INFO info;

	m_handle = GetStdHandle(STD_OUTPUT_HANDLE);

	if (GetConsoleScreenBufferInfo(m_handle, &info))
	{
		m_color = info.wAttributes;
	}
	else
	{
		m_color = 0;
	}
}

void DiagPrinterFancy::operator()(const Diag &d)
{
	PrintSev(d, "");

	if (d.caret.IsValid())
	{
		printf("%d:%d", d.caret.line, d.caret.column);
	}

	if (d.range.IsValid() && d.caret.IsValid())
	{
		printf(" (%d:%d to %d:%d)", d.range.beg.line, d.range.beg.column, d.range.end.line, d.range.end.column);
	}

	if (d.range.IsValid() && !d.caret.IsValid())
	{
		printf("%d:%d to %d:%d", d.range.beg.line, d.range.beg.column, d.range.end.line, d.range.end.column);
	}

	printf(": %s\n", d.message.c_str());

	Range range;

	if (d.range.IsValid())
	{
		range = d.range;
	}
	else if (d.caret.IsValid())
	{
		range = Range(d.caret, 1);
	}

	for (int line = range.beg.line; line <= range.end.line; line++)
	{
		if (range.end.line - range.beg.line > 1)
		{
			if (line == range.beg.line)
			{
				PrintSev(d, "starting at line %d with:\n", line);
			}
			else if (line == range.end.line)
			{
				PrintSev(d, "ending at line %d with:\n", line);
			}
			else
			{
				continue;
			}
		}

		auto linest = m_source.GetLine(line);
		auto marker = linest;
		auto begCol = line == range.beg.line ? range.beg.column : (unsigned int)1;
		auto endCol = line == range.end.line ? range.end.column : linest.length() + 1;

		// The marker might extend one character past the line end.
		marker += ' ';

		for (unsigned int j = 0; j < marker.size(); j++)
		{
			if (d.caret.IsValid() && d.caret.line == line && j + 1 == d.caret.column)
			{
				marker[j] = '^';
			}
			else if (j + 1 >= begCol && j + 1 < endCol)
			{
				marker[j] = '~';
			}
			else if (marker[j] != ' ' || marker[j] != '\t')
			{
				marker[j] = ' ';
			}
		}

		PrintSev(d, ">>> %s\n", linest.c_str());
		PrintSev(d, ">>> ");

		auto prev = SetColor(GetSeverityColor(d.severity));
		printf("%s\n", marker.c_str());
		SetColor(prev);
	}

	printf("\n");
}

Mond::string Mond::DiagPrinterFancy::BuildMarker(const Diag &d, int line)
{
	int beg = -1;
	int end = -1;

	return "";
}

void DiagPrinterFancy::PrintSev(const Diag &d, const char *fmt, ...)
{
	char buffer[1024];

	va_list ap;
	va_start(ap, fmt);
	vsnprintf_s(buffer, 1024, fmt, ap);
	va_end(ap);

	printf("[");

	WORD prev = SetColor(GetSeverityColor(d.severity));
	printf("%s", GetSeverityName(d.severity));
	SetColor(prev);

	printf("] %s", buffer);
}

WORD DiagPrinterFancy::SetColor(WORD color)
{
	auto prev = m_color;
	SetConsoleTextAttribute(m_handle, color);
	return prev;
}

WORD DiagPrinterFancy::GetSeverityColor(Severity s)
{
	switch (s)
	{
	case Info:
		return FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN;
	case Warning:
		return FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
	case Error:
		return FOREGROUND_INTENSITY | FOREGROUND_RED;
	}

	throw invalid_argument("unknown diagnostic severity");
}