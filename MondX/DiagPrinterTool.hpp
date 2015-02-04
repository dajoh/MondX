#ifndef MOND_DIAG_PRINTER_TOOL_HPP
#define MOND_DIAG_PRINTER_TOOL_HPP

#include "Diag.hpp"

namespace Mond
{
	class DiagPrinterTool
	{
	public:
		DiagPrinterTool();

		void operator()(const Diag &d);
	};
}

#endif