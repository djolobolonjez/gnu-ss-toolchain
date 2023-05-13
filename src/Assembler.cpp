#include "../inc/Assembler.hpp"
#include "../inc/AssemblyDirectives.hpp"
#include "../inc/SymbolTable.hpp"
#include "../inc/defs.h"

Assembler& Assembler::getInstance() {
  static Assembler instance;
  return instance;
}

AssemblyDirectives& Assembler::directives() {
  return AssemblyDirectives::getInstance();
}

void Assembler::printSymbolTable() {
  SymbolTable& symtab = SymbolTable::getInstance();

  cout << endl << "#.symtab" << endl;

  cout << "Num" << "\t" << "Value" << "\t" << "Size" << "\t" << "Type" << "\t" 
  << "Bind" << "\t" << "Index" << "\t" << "Name";
  cout << endl;

  for (int i = 0; i < symtab.size(); i++) {
    SymbolTableEntry*& stEntry = symtab[i];
    cout << i << "\t";
    cout << stEntry->value << "\t";
    cout << stEntry->size << "\t";

    cout << (ST_TYPE(stEntry->info) == STTYPE_NOTYPE ? "NOTYPE" : "SECTION");
    cout << "\t";

    cout << (ST_BIND(stEntry->info) == STBIND_GLOBAL ? "GLOB" : "LOC");
    cout << "\t";

    if (stEntry->index == UND) {
      cout << "UND";
    }
    else {
      cout << stEntry->index;
    }
    cout << "\t" << stEntry->name << endl;
  }
  
}