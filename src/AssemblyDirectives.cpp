#include "../inc/AssemblyDirectives.hpp"
#include "../inc/SymbolTable.hpp"
#include "../inc/defs.h"

AssemblyDirectives& AssemblyDirectives::getInstance() {
  static AssemblyDirectives instance;
  return instance;
}

void AssemblyDirectives::global(AssemblyLineArguments* args) {
  // ovo je samo test primer ove direktive!!!
  
  ArgVector arguments = *(args->args);
  SymbolTable& symtab = SymbolTable::getInstance();
  for (auto sptr : arguments) {
    symtab.addSymbol(*sptr, 0, ST_INFO(STBIND_GLOBAL, STTYPE_NOTYPE), UND, 0);
  }
}

void AssemblyDirectives::end() {
  cout << "End of assembly file" << endl;
}