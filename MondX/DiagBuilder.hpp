#ifndef MOND_DIAG_BUILDER_HPP
#define MOND_DIAG_BUILDER_HPP

#include "Diag.hpp"
#include "Token.hpp"

namespace Mond
{
	enum DiagSentinel
	{
		DiagEnd
	};

	typedef function<void(const Diag &)> DiagObserver;

	class DiagBuilder
	{
	public:
		DiagBuilder(DiagObserver fn);

		DiagBuilder &operator<<(Pos caret);
		DiagBuilder &operator<<(Range range);
		DiagBuilder &operator<<(Severity s);
		DiagBuilder &operator<<(DiagMessage m);

		DiagBuilder &operator<<(int n);
		DiagBuilder &operator<<(uint32_t c);
		DiagBuilder &operator<<(TokenType t);
		DiagBuilder &operator<<(const string &s);

		DiagBuilder &operator<<(DiagSentinel b);
	private:
		void WriteRest();
		void WriteUntilFormatter(char expect);

		int m_pos;
		const char *m_fmt;

		Diag m_diag;
		stringstream m_msg;
		DiagObserver m_func;
	};
}

#endif