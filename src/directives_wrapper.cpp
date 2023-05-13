#include "../inc/directives_wrapper.h"
#include "../inc/types.h"
#include "../inc/Assembler.hpp"
#include "../inc/AssemblyDirectives.hpp"
#include <iostream>

using namespace std;

void _asm_dir_global(AssemblyLineArguments* args) {
  Assembler::getInstance().directives().global(args);
}

void _asm_dir_end() {
  Assembler::getInstance().directives().end();
  Assembler::getInstance().printSymbolTable();
}