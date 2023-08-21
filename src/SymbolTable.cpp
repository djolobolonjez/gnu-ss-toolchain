#include "../inc/SymbolTable.hpp"
#include "../inc/defs.h"

SymbolTable::SymbolTable() {
  addSymbol("undefined", 0, ST_INFO(STBIND_LOCAL, STTYPE_SECTION), UND, 0);
}

void SymbolTable::addSymbol(string name, ulong value, byte info, int index, ulong size) {
  entryIndexMap[name] = symtab.size();
  symtab.push_back(new SymbolTableEntry(value, name, info, index, size));
}

SymbolTable& SymbolTable::getInstance() {
  static SymbolTable instance;
  return instance;
}