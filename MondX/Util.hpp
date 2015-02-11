#ifndef MOND_UTIL_HPP
#define MOND_UTIL_HPP

#include <cstdint>
#include <deque>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <functional>
#include <unordered_map>

namespace Mond
{
	using std::hex;
	using std::pair;
	using std::deque;
	using std::string;
	using std::vector;
	using std::function;
	using std::ifstream;
	using std::shared_ptr;
	using std::unique_ptr;
	using std::logic_error;
	using std::stringstream;
	using std::unordered_map;
	using std::invalid_argument;

	// -----------------------------------------------------------------------
	// Utility structures
	// -----------------------------------------------------------------------

	struct Pos
	{
		Pos();
		Pos(int line, int column);

		bool IsValid() const;

		bool operator==(const Pos &other) const;
		bool operator!=(const Pos &other) const;

		int line;
		int column;
	};

	struct Slice
	{
		Slice();
		Slice(int pos);
		Slice(int beg, int end);

		bool IsValid() const;

		int beg;
		int end;
	};

	struct Range
	{
		Range();
		Range(Pos beg, Pos end);
		Range(Pos beg, int length);

		bool IsValid() const;

		Pos beg;
		Pos end;
	};

	// -----------------------------------------------------------------------
	// Pos implementation
	// -----------------------------------------------------------------------

	inline Pos::Pos() : line(-1), column(-1)
	{
	}

	inline Pos::Pos(int line, int column) : line(line), column(column)
	{
	}

	inline bool Pos::IsValid() const
	{
		return line != -1 && column != -1;
	}

	inline bool Pos::operator==(const Pos &other) const
	{
		return line == other.line && column == other.column;
	}

	inline bool Pos::operator!=(const Pos &other) const
	{
		return line != other.line || column != other.column;
	}

	// -----------------------------------------------------------------------
	// Slice implementation
	// -----------------------------------------------------------------------

	inline Slice::Slice() : beg(-1), end(-1)
	{
	}

	inline Slice::Slice(int pos) : beg(pos), end(pos + 1)
	{
	}

	inline Slice::Slice(int beg, int end) : beg(beg), end(end)
	{
	}

	inline bool Slice::IsValid() const
	{
		return beg != -1 && end != -1;
	}

	// -----------------------------------------------------------------------
	// Range implementation
	// -----------------------------------------------------------------------

	inline Range::Range()
	{
	}

	inline Range::Range(Pos beg, Pos end) : beg(beg), end(end)
	{
	}

	inline Range::Range(Pos beg, int length) : beg(beg), end(beg.line, beg.column + length)
	{
	}

	inline bool Range::IsValid() const
	{
		return beg.IsValid() && end.IsValid();
	}
}

#endif