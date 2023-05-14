#ifndef SYMBOL_TABLE_HPP_
#define SYMBOL_TABLE_HPP_

#include "types.h"
#include <map>

class SymbolTable {
private:
  SymbolTable();
  
  vector<SymbolTableEntry*> symtab;
  map<string, int> entryIndexMap;

  int symbolID = 0; 

public:
  static SymbolTable& getInstance();
  
  int getSymbolID() { return symbolID++; }

  int size() const { return symtab.size(); }
  bool foundEntryByName(string name) const { return entryIndexMap.find(name) != entryIndexMap.end(); } 
  int getIndexOfEntry(string name) { return entryIndexMap[name]; }

  void addSymbol(string name, ulong value, byte info, int index, ulong size);

  SymbolTableEntry*& operator[](int index) { return symtab[index]; }

};

#endif // SYMBOL_TABLE_HPP_
