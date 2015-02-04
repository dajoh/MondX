#ifndef MOND_DIAG_HPP
#define MOND_DIAG_HPP

#include "Util.hpp"
#include "DiagMessage.hpp"

namespace Mond
{
	enum Severity
	{
		Info,
		Warning,
		Error
	};

	struct Diag
	{
		Pos caret;
		Range range;
		string message;
		Severity severity;
		DiagMessage messageId;
	};

	const char *GetSeverityName(Severity s);
}

#endif