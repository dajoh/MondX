#ifndef MOND_DIAG_PRINTER_FANCY_CORE_HPP
#define MOND_DIAG_PRINTER_FANCY_CORE_HPP

#include "Diag.hpp"
#include "Source.hpp"

namespace Mond
{
	class DiagPrinterFancyCore
	{
	public:
		DiagPrinterFancyCore(Source &source);

		void operator()(const Diag &d);
	private:
		void PrintSev(const Diag &d, const char *fmt, ...);

		virtual void SetColor(Severity s) = 0;
		virtual void ResetColor() = 0;

		Source &m_source;
	};
}

#endif