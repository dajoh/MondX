#ifndef MOND_DIAG_PRINTER_FANCY_HPP
#define MOND_DIAG_PRINTER_FANCY_HPP

#include <Windows.h>
#include "Diag.hpp"
#include "Source.hpp"

namespace Mond
{
	// TODO: Cross-platform fancy printer.
	class DiagPrinterFancy
	{
	public:
		DiagPrinterFancy(Source &source);

		void operator()(const Diag &d);
	private:
		string BuildMarker(const Diag &d, int line);

		void PrintSev(const Diag &d, const char *fmt, ...);
		WORD SetColor(WORD color);
		WORD GetSeverityColor(Severity s);

		WORD m_color;
		HANDLE m_handle;
		Source &m_source;
	};
}

#endif