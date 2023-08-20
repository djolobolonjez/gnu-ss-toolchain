all: 
	bison -d misc/parser.y 
	flex misc/lexer.l 

parse:
	g++ lexer.cpp parser.cpp src/instructions_wrapper.cpp src/directives_wrapper.cpp src/utils.cpp src/AssemblyInstructions.cpp src/AssemblyDirectives.cpp src/Assembler.cpp src/SymbolTable.cpp src/Section.cpp src/SectionTable.cpp -o asembler

link:
	g++ src/Linker.cpp src/linker_start.cpp src/Section.cpp src/SectionTable.cpp src/SymbolTable.cpp src/utils.cpp src/AssemblyDirectives.cpp src/Assembler.cpp src/AssemblyInstructions.cpp -o linker

clean:
	rm -f parser.cpp lexer.cpp parser.hpp lexer.hpp ./asembler

.PHONY:
	clean 
	parse