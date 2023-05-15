#include "../inc/directives_wrapper.h"
#include "../inc/types.h"
#include "../inc/Assembler.hpp"
#include "../inc/AssemblyDirectives.hpp"
#include <iostream>

using namespace std;

void _asm_dir_global(AssemblyLineArguments* args) {
    Assembler::getInstance().directives().global(args);
}

void _asm_dir_extern(AssemblyLineArguments* args) {
  Assembler::getInstance().directives().ext(args);
} 

void _asm_dir_section(string* arg) {
  Assembler::getInstance().directives().section(arg);
}

void _asm_dir_word(AssemblyLineArguments* args) {
  Assembler::getInstance().directives().word(args);
}

void _asm_dir_skip(string* arg) {
  int base = 10;
  if (arg->substr(0,2) == "0x") {
    base = 16;
  }
  else if (arg->substr(0,1) == "0") {
    base = 8;
  }

  Assembler::getInstance().directives().skip(*arg, base);
}

void _asm_dir_ascii(string* arg) {
  Assembler::getInstance().directives().ascii(*arg);
}

void _asm_label(string* arg) {
  Assembler::getInstance().label(arg);
}

void _asm_dir_end() {
  Assembler::getInstance().directives().end();
  Assembler::getInstance().printSymbolTable();
}