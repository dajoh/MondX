#include <fstream>
#include "../MondX/Sema.hpp"
#include "../MondX/Parser.hpp"
#include "../MondX/DiagPrinterTool.hpp"

#ifdef _WIN32
#include "../MondX/DiagPrinterFancyWin32.hpp"
#else
#include "../MondX/DiagPrinterFancyUnix.hpp"
#endif

using namespace std;
using namespace Mond;

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("usage: mond-lint [-ftool] <filename>\n");
		return 1;
	}

	string filename;
	if (argc == 3)
	{
		filename = argv[2];
	}
	else
	{
		filename = argv[1];
	}

	ifstream file(filename);
	if (!file.is_open())
	{
		printf("error: couldn't open %s\n", filename.c_str());
		return 1;
	}

	string str((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

	StringSource src(str.c_str());
	DiagObserver obs;

	if (argc == 3 && strcmp(argv[1], "-ftool") == 0)
	{
		obs = DiagPrinterTool();
	}
	else
	{
		obs = DiagPrinterFancy(src);
	}

	DiagBuilder diag(obs);
	Lexer lexer(diag, src);
	Parser parser(diag, src, lexer);
	Sema sema(diag);

	sema.Run(parser.ParseFile());
	return 0;
}
