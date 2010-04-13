FILES=main.cpp message.cpp memory_pool.cpp string_pool.cpp compiler.cpp document.cpp parser/parser.cpp parser/globals.cpp parser/statements.cpp parser/expressions.cpp tree/globals2.cpp tree/expressions2.cpp tree/statements2.cpp tree/types.cpp tree/symbols.cpp tree/printer.cpp lexer/lexer.cpp lexer/lexeme.cpp lexer/handlers.cpp lexer/numbers.cpp lexer/strings.cpp

legion : $(FILES)
	g++ -std=gnu++0x -g -Wall -pipe -D VALGRIND -D AST_DEBUG $(FILES) -o legion
