#include "../MondX/Sema.hpp"
#include "../MondX/Parser.hpp"
#include "../MondX/DiagPrinterTool.hpp"

#ifdef _WIN32
#include "../MondX/DiagPrinterFancyWin32.hpp"
#else
#include "../MondX/DiagPrinterFancyUnix.hpp"
#endif

using namespace Mond;

void usage()
{
	printf("usage: mondx-lint [-f fancy|tool] [-b <builtin.mnd>] <filename>\n");
}

int main(int argc, char *argv[])
{
	string lintFile;
	string diagFormat = "fancy";
	string builtinFile;

	if (argc < 2)
	{
		usage();
		return 1;
	}

	for (int i = 1; i < argc; i++)
	{
		string arg = argv[i];

		if (arg == "-f" || arg == "-b")
		{
			i++;

			if (i >= argc - 1)
			{
				usage();
				return 1;
			}

			if (arg == "-f")
			{
				diagFormat = argv[i];
			}
			else if (arg == "-b")
			{
				builtinFile = argv[i];
			}
		}
		else if (i == argc - 1)
		{
			lintFile = argv[i];
		}
	}

	ScopePtr builtinScope;
	StmtPtrList builtinStmts;

	if (builtinFile != "")
	{
		FileSource source(builtinFile);

		DiagBuilder diag([](const Diag &){});
		Lexer lexer(diag, source);
		Sema sema(diag, NULL);
		Parser parser(diag, source, lexer, sema);

		builtinStmts = parser.ParseFile();
		builtinScope = sema.RootScope();
	}

	FileSource source(lintFile);
	DiagObserver observer;

	if (diagFormat == "tool")
	{
		observer = DiagPrinterTool();
	}
	else if (diagFormat == "fancy")
	{
		observer = DiagPrinterFancy(source);
	}
	else
	{
		usage();
		return 1;
	}

	DiagBuilder diag(observer);
	Lexer lexer(diag, source);
	Sema sema(diag, builtinScope);
	Parser parser(diag, source, lexer, sema);
	parser.ParseFile();

	return 0;
}