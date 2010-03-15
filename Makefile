FILES=main.cpp memory_pool.cpp string_pool.cpp compiler.cpp parser/parser.cpp lexer/lexer.cpp lexer/numbers.cpp

legion : $(FILES)
	g++ -g -Wall -fstack-protector-all -pipe -D VALGRIND $(FILES) -o legion
