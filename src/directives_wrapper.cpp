#include "../inc/directives_wrapper.h"
#include "../inc/types.h"
#include "../inc/Assembler.hpp"
#include "../inc/AssemblyDirectives.hpp"
#include "../inc/utils.hpp"
#include <iostream>

using namespace std;

void _asm_dir_global(AssemblyLineArguments* args, bool secondPass) {
  if (secondPass == false) {
    Assembler::getInstance().directives().global(args);
  }
}

void _asm_dir_extern(AssemblyLineArguments* args, bool secondPass) {
  if (secondPass == false) {
    Assembler::getInstance().directives().ext(args);
  }
} 

void _asm_dir_section(string* arg, bool secondPass) {
  if (secondPass == false) {
    Assembler::getInstance().directives().sectionPassOne(arg);
  }
  else {
    Assembler::getInstance().directives().sectionPassTwo(arg);
  }
}

void _asm_dir_word(AssemblyLineArguments* args, bool secondPass) {
  if (secondPass) {
    Assembler::getInstance().directives().wordPassTwo(args);
  }
  else {
    Assembler::getInstance().directives().wordPassOne(args);
  }
}

void _asm_dir_skip(string* arg, bool secondPass) {
  int base = Utils::findBase(*arg);

  if (secondPass) {
    Assembler::getInstance().directives().skipPassTwo(*arg, base);
  }
  else {
    Assembler::getInstance().directives().skipPassOne(*arg, base);
  }
}

void _asm_dir_ascii(string* arg, bool secondPass) {
  if (secondPass) {
    Assembler::getInstance().directives().asciiPassTwo(*arg);
  } 
  else {
    Assembler::getInstance().directives().asciiPassOne(*arg);
  }
}

void _asm_label(string* arg, bool secondPass) {
  if (secondPass == false) {
    Assembler::getInstance().label(arg);
  }
}

void _asm_dir_end(bool secondPass) {
  if (secondPass) {
    Assembler::getInstance().directives().end();
  }
  else {
    Assembler::getInstance().sectionReset();
  }
  Assembler::getInstance().printSymbolTable();
}