#include "DiagPrinterTool.hpp"

using namespace Mond;

DiagPrinterTool::DiagPrinterTool()
{
}

void DiagPrinterTool::operator()(const Diag &d)
{
	if (d.caret.IsValid())
	{
		printf("%d:%d: ", d.caret.line, d.caret.column);
	}

	if (d.range.IsValid())
	{
		printf("%d:%d-%d:%d: ", d.range.beg.line, d.range.beg.column, d.range.end.line, d.range.end.column);
	}

	printf("%s: %s\n", GetSeverityName(d.severity), d.message.c_str());
}