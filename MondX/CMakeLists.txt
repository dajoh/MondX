if (WIN32)
	set (MONDX_WIN32_SOURCES DiagPrinterFancy.cpp)
endif()

add_library (MondX
	Diag.cpp
	DiagBuilder.cpp
	DiagMessage.cpp
	DiagPrinterTool.cpp
	Lexer.cpp
	OperatorUtil.cpp
	Parser.cpp
	Sema.cpp
	Source.cpp
	Token.cpp
	Visitor.cpp
	${MONDX_WIN32_SOURCES})
