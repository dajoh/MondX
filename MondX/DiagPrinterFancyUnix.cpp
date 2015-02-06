#include "DiagPrinterFancyUnix.hpp"

using namespace Mond;

DiagPrinterFancy::DiagPrinterFancy(Source &source) : DiagPrinterFancyCore(source)
{
}

void DiagPrinterFancy::SetColor(Severity s)
{
	switch (s) {
	case Info:
		printf("\x1B[1;36m");
		break;
	case Warning:
		printf("\x1B[1;33m");
		break;
	case Error:
		printf("\x1B[1;31m");
		break;
	}
}

void DiagPrinterFancy::ResetColor()
{
		printf("\x1B[0m");
}