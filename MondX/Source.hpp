#ifndef MOND_SOURCE_HPP
#define MOND_SOURCE_HPP

#include "Util.hpp"

namespace Mond
{
	class Source
	{
	public:
		virtual string GetLine(int line) const = 0;
		virtual string GetSlice(Slice s) const = 0;
		virtual string GetRange(Range r) const = 0;

		virtual void Advance() = 0;
		virtual int Position() const = 0;

		virtual uint32_t Cur() const = 0;
		virtual uint32_t Peek() const = 0;
	};

	class StringSource : public Source
	{
	public:
		StringSource(const char *str);

		string GetLine(int line) const;
		string GetSlice(Slice s) const;
		string GetRange(Range r) const;

		void Advance();
		int Position() const;

		uint32_t Cur() const;
		uint32_t Peek() const;
	private:
		const char *m_beg;
		const char *m_ptr;
	};
}

#endif