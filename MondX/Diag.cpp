#include "Diag.hpp"

using namespace Mond;

const char *Mond::GetSeverityName(Severity s)
{
	switch (s)
	{
	case Info:
		return "info";
	case Warning:
		return "warning";
	case Error:
		return "error";
	}

	throw invalid_argument("unknown diagnostic severity");
}