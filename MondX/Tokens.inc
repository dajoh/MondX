#ifndef MOND_TOKEN
#define MOND_TOKEN(n, s)
#define UNDEFINE_MOND_TOKEN
#endif

#ifndef MOND_PUNCT
#define MOND_PUNCT(n, s)
#define UNDEFINE_MOND_PUNCT
#endif

#ifndef MOND_KEYWORD
#define MOND_KEYWORD(n, s)
#define UNDEFINE_MOND_KEYWORD
#endif

#ifndef MOND_OPERATOR
#define MOND_OPERATOR(n, s)
#define UNDEFINE_MOND_OPERATOR
#endif

#ifndef MOND_OPERATOR1
#define MOND_OPERATOR1(c, a)
#define MOND_OPERATORN(c, a, b)
#define UNDEFINE_MOND_OPERATORN
#endif

MOND_TOKEN(EndOfFile,     "end of file")
MOND_TOKEN(Unknown,       "unknown")
MOND_TOKEN(Completion,    "code completion")
MOND_TOKEN(WhiteSpace,    "white space")
MOND_TOKEN(LineComment,   "line comment")
MOND_TOKEN(BlockComment,  "block comment")

MOND_TOKEN(Identifier,    "identifier")
MOND_TOKEN(StringLiteral, "string literal")
MOND_TOKEN(NumberLiteral, "number literal")

MOND_PUNCT(Colon,         ":")
MOND_PUNCT(Comma,         ",")
MOND_PUNCT(Semicolon,     ";")
MOND_PUNCT(LeftParen,     "(")
MOND_PUNCT(RightParen,    ")")
MOND_PUNCT(LeftBrace,     "{")
MOND_PUNCT(RightBrace,    "}")
MOND_PUNCT(LeftBracket,   "[")
MOND_PUNCT(RightBracket,  "]")

MOND_KEYWORD(Global,    "global")
MOND_KEYWORD(Null,      "null")
MOND_KEYWORD(Undefined, "undefined")
MOND_KEYWORD(True,      "true")
MOND_KEYWORD(False,     "false")
MOND_KEYWORD(NaN,       "NaN")
MOND_KEYWORD(Infinity,  "Infinity")
MOND_KEYWORD(Fun,       "fun")
MOND_KEYWORD(Seq,       "seq")
MOND_KEYWORD(Var,       "var")
MOND_KEYWORD(Const,     "const")
MOND_KEYWORD(Yield,     "yield")
MOND_KEYWORD(Return,    "return")
MOND_KEYWORD(If,        "if")
MOND_KEYWORD(Else,      "else")
MOND_KEYWORD(Do,        "do")
MOND_KEYWORD(For,       "for")
MOND_KEYWORD(While,     "while")
MOND_KEYWORD(Foreach,   "foreach")
MOND_KEYWORD(In,        "in")
MOND_KEYWORD(NotIn,     "!in") // TODO: fix
MOND_KEYWORD(Break,     "break")
MOND_KEYWORD(Continue,  "continue")
MOND_KEYWORD(Switch,    "switch")
MOND_KEYWORD(Case,      "case")
MOND_KEYWORD(Default,   "default")

MOND_OPERATOR(Add,                 "+")   MOND_OPERATOR1('+', Add)
MOND_OPERATOR(Subtract,            "-")   MOND_OPERATOR1('-', Subtract)
MOND_OPERATOR(Multiply,            "*")   MOND_OPERATOR1('*', Multiply)
MOND_OPERATOR(Divide,              "/")   MOND_OPERATOR1('/', Divide)
MOND_OPERATOR(Modulo,              "%")   MOND_OPERATOR1('%', Modulo)
MOND_OPERATOR(Exponent,            "**")  MOND_OPERATORN('*', Multiply, Exponent)
MOND_OPERATOR(Increment,           "++")  MOND_OPERATORN('+', Add, Increment)
MOND_OPERATOR(Decrement,           "--")  MOND_OPERATORN('-', Subtract, Decrement)
MOND_OPERATOR(BitLeftShift,        "<<")  MOND_OPERATORN('<', LessThan, BitLeftShift)
MOND_OPERATOR(BitRightShift,       ">>")  MOND_OPERATORN('>', GreaterThan, BitRightShift)
MOND_OPERATOR(BitAnd,              "&")   MOND_OPERATOR1('&', BitAnd)
MOND_OPERATOR(BitOr,               "|")   MOND_OPERATOR1('|', BitOr)
MOND_OPERATOR(BitXor,              "^")   MOND_OPERATOR1('^', BitXor)
MOND_OPERATOR(BitNot,              "~")   MOND_OPERATOR1('~', BitNot)

MOND_OPERATOR(Assign,              "=")   MOND_OPERATOR1('=', Assign)
MOND_OPERATOR(AddAssign,           "+=")  MOND_OPERATORN('=', Add, AddAssign)
MOND_OPERATOR(SubtractAssign,      "-=")  MOND_OPERATORN('=', Subtract, SubtractAssign)
MOND_OPERATOR(MultiplyAssign,      "*=")  MOND_OPERATORN('=', Multiply, MultiplyAssign)
MOND_OPERATOR(DivideAssign,        "/=")  MOND_OPERATORN('=', Divide, DivideAssign)
MOND_OPERATOR(ModuloAssign,        "%=")  MOND_OPERATORN('=', Modulo, ModuloAssign)
MOND_OPERATOR(ExponentAssign,      "**=") MOND_OPERATORN('=', Exponent, ExponentAssign)
MOND_OPERATOR(BitLeftShiftAssign,  "<<=") MOND_OPERATORN('=', BitLeftShift, BitLeftShiftAssign)
MOND_OPERATOR(BitRightShiftAssign, ">>=") MOND_OPERATORN('=', BitRightShift, BitRightShiftAssign)
MOND_OPERATOR(BitAndAssign,        "&=")  MOND_OPERATORN('=', BitAnd, BitAndAssign)
MOND_OPERATOR(BitOrAssign,         "|=")  MOND_OPERATORN('=', BitOr, BitOrAssign)
MOND_OPERATOR(BitXorAssign,        "^=")  MOND_OPERATORN('=', BitXor, BitXorAssign)

MOND_OPERATOR(Not,                 "!")   MOND_OPERATOR1('!', Not)
MOND_OPERATOR(EqualTo,             "==")  MOND_OPERATORN('=', Assign, EqualTo)
MOND_OPERATOR(NotEqualTo,          "!=")  MOND_OPERATORN('=', Not, NotEqualTo)
MOND_OPERATOR(GreaterThan,         ">")   MOND_OPERATOR1('>', GreaterThan)
MOND_OPERATOR(GreaterThanOrEqual,  ">=")  MOND_OPERATORN('=', GreaterThan, GreaterThanOrEqual)
MOND_OPERATOR(LessThan,            "<")   MOND_OPERATOR1('<', LessThan)
MOND_OPERATOR(LessThanOrEqual,     "<=")  MOND_OPERATORN('=', LessThan, LessThanOrEqual)
MOND_OPERATOR(ConditionalAnd,      "&&")  MOND_OPERATORN('&', BitAnd, ConditionalAnd)
MOND_OPERATOR(ConditionalOr,       "||")  MOND_OPERATORN('|', BitOr, ConditionalOr)

MOND_OPERATOR(Dot,                 ".")   MOND_OPERATOR1('.', Dot)
MOND_OPERATOR(DotDot,              "..")  MOND_OPERATORN('.', Dot, DotDot) // TODO: fix
MOND_OPERATOR(Ellipsis,            "...") MOND_OPERATORN('.', DotDot, Ellipsis)
MOND_OPERATOR(QuestionMark,        "?")   MOND_OPERATOR1('?', QuestionMark)
MOND_OPERATOR(Pointy,              "->")  MOND_OPERATORN('>', Subtract, Pointy)
MOND_OPERATOR(Pipeline,            "|>")  MOND_OPERATORN('>', BitOr, Pipeline)

#ifdef UNDEFINE_MOND_TOKEN
#undef UNDEFINE_MOND_TOKEN
#undef MOND_TOKEN
#endif

#ifdef UNDEFINE_MOND_PUNCT
#undef UNDEFINE_MOND_PUNCT
#undef MOND_PUNCT
#endif

#ifdef UNDEFINE_MOND_KEYWORD
#undef UNDEFINE_MOND_KEYWORD
#undef MOND_KEYWORD
#endif

#ifdef UNDEFINE_MOND_OPERATOR
#undef UNDEFINE_MOND_OPERATOR
#undef MOND_OPERATOR
#endif

#ifdef UNDEFINE_MOND_OPERATORN
#undef UNDEFINE_MOND_OPERATORN
#undef MOND_OPERATOR1
#undef MOND_OPERATORN
#endif
