all: 
	bison -d misc/parser.y 
	flex misc/lexer.l 

asm:
	g++ lexer.cpp parser.cpp src/instructions_wrapper.cpp src/directives_wrapper.cpp src/utils.cpp src/AssemblyInstructions.cpp src/AssemblyDirectives.cpp src/Assembler.cpp src/SymbolTable.cpp src/Section.cpp src/SectionTable.cpp -o assembler

link:
	g++ src/Linker.cpp src/linker_start.cpp src/Section.cpp src/SectionTable.cpp src/SymbolTable.cpp src/utils.cpp src/AssemblyDirectives.cpp src/Assembler.cpp src/AssemblyInstructions.cpp -o linker

clean:
	rm -f parser.cpp lexer.cpp parser.hpp lexer.hpp ./assembler

.PHONY:
	clean 
	parse