#ifndef MOND_DIAG_PRINTER_FANCY_WIN32_HPP
#define MOND_DIAG_PRINTER_FANCY_WIN32_HPP

#include <Windows.h>
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

		WORD m_plainColor;
		HANDLE m_handle;
	};
}

#endif