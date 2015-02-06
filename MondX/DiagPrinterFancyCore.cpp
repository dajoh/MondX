#include <cstdarg>
#include "DiagPrinterFancyCore.hpp"

using namespace Mond;

DiagPrinterFancyCore::DiagPrinterFancyCore(Source &source) : m_source(source)
{
}

void DiagPrinterFancyCore::operator()(const Diag &d)
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

		SetColor(d.severity);
		printf("%s\n", marker.c_str());
		ResetColor();
	}

	printf("\n");
}

void DiagPrinterFancyCore::PrintSev(const Diag &d, const char *fmt, ...)
{
	char buffer[1024];

	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buffer, 1024, fmt, ap);
	va_end(ap);

	printf("[");

	SetColor(d.severity);
	printf("%s", GetSeverityName(d.severity));
	ResetColor();

	printf("] %s", buffer);
}