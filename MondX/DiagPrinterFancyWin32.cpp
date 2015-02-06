#include "DiagPrinterFancyWin32.hpp"

using namespace Mond;

DiagPrinterFancy::DiagPrinterFancy(Source &source) : DiagPrinterFancyCore(source)
{
	CONSOLE_SCREEN_BUFFER_INFO info;

	m_handle = GetStdHandle(STD_OUTPUT_HANDLE);

	if (GetConsoleScreenBufferInfo(m_handle, &info))
	{
		m_plainColor = info.wAttributes;
	}
	else
	{
		m_plainColor = 0;
	}
}

void DiagPrinterFancy::SetColor(Severity s)
{
	WORD color;

	switch (s)
	{
	case Info:
		color = FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN;
	case Warning:
		color = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
	case Error:
		color = FOREGROUND_INTENSITY | FOREGROUND_RED;
	}

	SetConsoleTextAttribute(m_handle, color);
}

void DiagPrinterFancy::ResetColor()
{
	SetConsoleTextAttribute(m_handle, m_plainColor);
}