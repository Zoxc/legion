FILES=main.cpp range.cpp memory_pool.cpp string_pool.cpp compiler.cpp document.cpp parser/parser.cpp parser/globals.cpp parser/statements.cpp parser/expressions.cpp tree/scope.cpp tree/types.cpp tree/symbols.cpp tree/printer.cpp lexer/lexer.cpp lexer/lexeme.cpp lexer/handlers.cpp lexer/numbers.cpp lexer/strings.cpp

legion : $(FILES)
	g++ -g -Wall -pipe -D VALGRIND $(FILES) -o legion
