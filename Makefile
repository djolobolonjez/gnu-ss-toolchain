all: 
	bison -d misc/parser.y 
	flex misc/lexer.l 

parse:
	g++ lexer.cpp parser.cpp src/directives_wrapper.cpp src/utils.cpp src/AssemblyDirectives.cpp src/Assembler.cpp src/SymbolTable.cpp src/Section.cpp src/SectionTable.cpp -o proba

clean:
	rm -f parser.cpp lexer.cpp parser.hpp lexer.hpp ./proba

.PHONY:
	clean 
	parse