FILES=main.cpp memory_pool.cpp string_pool.cpp compiler.cpp document.cpp parser/parser.cpp tree/scope.cpp lexer/lexer.cpp lexer/lexeme.cpp lexer/numbers.cpp lexer/strings.cpp lexer/handlers.cpp

legion : $(FILES)
	g++ -g -Wall -fstack-protector-all -pipe -D VALGRIND $(FILES) -o legion
