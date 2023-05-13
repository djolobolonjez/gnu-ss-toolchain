parse:
	g++ lexer.cpp parser.cpp src/directives_wrapper.cpp src/utils.cpp src/AssemblyDirectives.cpp src/Assembler.cpp src/SymbolTable.cpp -o proba

clean:
	rm -f parser.cpp lexer.cpp parser.hpp lexer.hpp

.PHONY:
	clean 
	parse