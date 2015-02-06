if (WIN32)
	set (MONDX_PLATFORM_SOURCES DiagPrinterFancyWin32.cpp DiagPrinterFancyWin32.hpp)
else()
	set (MONDX_PLATFORM_SOURCES DiagPrinterFancyUnix.cpp DiagPrinterFancyUnix.hpp)
endif()

add_library (MondX
	AST.hpp
	Diag.cpp
	Diag.hpp
	DiagBuilder.cpp
	DiagBuilder.hpp
	DiagMessage.cpp
	DiagMessage.hpp
	DiagPrinterFancyCore.cpp
	DiagPrinterFancyCore.hpp
	DiagPrinterTool.cpp
	DiagPrinterTool.hpp
	Lexer.cpp
	Lexer.hpp
	OperatorUtil.cpp
	OperatorUtil.hpp
	Parser.cpp
	Parser.hpp
	Sema.cpp
	Sema.hpp
	Source.cpp
	Source.hpp
	Token.cpp
	Token.hpp
	Util.hpp
	Visitor.cpp
	Visitor.hpp
	${MONDX_PLATFORM_SOURCES})
