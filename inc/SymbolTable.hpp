#ifndef SYMBOL_TABLE_HPP_
#define SYMBOL_TABLE_HPP_

#include "types.h"

class SymbolTable {
private:
  SymbolTable();
  vector<SymbolTableEntry*> symtab;

public:
  static SymbolTable& getInstance();

  int size() const { return symtab.size(); }

  void addSymbol(string name, ulong value, byte info, int index, ulong size);
  
  SymbolTableEntry*& operator[](int index) { return symtab[index]; }

};

#endif // SYMBOL_TABLE_HPP_
