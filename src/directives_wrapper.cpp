#include "../inc/directives_wrapper.h"
#include "../inc/types.h"
#include "../inc/Assembler.hpp"
#include "../inc/AssemblyDirectives.hpp"
#include <iostream>

using namespace std;

void _asm_dir_global(AssemblyLineArguments* args) {
  Assembler::getInstance().directives().global(args);
}

int _asm_dir_extern(AssemblyLineArguments* args) {
  return Assembler::getInstance().directives().ext(args);
} 

void _asm_dir_section(string* arg) {
  Assembler::getInstance().directives().section(arg);
}

int _asm_label(string* arg) {
  return Assembler::getInstance().label(arg);
}

void _asm_dir_end() {
  Assembler::getInstance().directives().end();
  Assembler::getInstance().printSymbolTable();
}