#ifndef MOND_DIAG_PRINTER_FANCY_UNIX_HPP
#define MOND_DIAG_PRINTER_FANCY_UNIX_HPP

#include "DiagPrinterFancyCore.hpp"

namespace Mond
{
	class DiagPrinterFancy : public DiagPrinterFancyCore
	{
	public:
		DiagPrinterFancy(Source &source);
	private:
		void SetColor(Severity s);
		void ResetColor();
	};
}

#endif