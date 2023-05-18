#ifndef RELOCATION_TABLE_HPP_
#define RELOCATION_TABLE_HPP_

#include "types.h"
#include "defs.h"

class RelocationTable {
private:
  vector<RelocationTableEntry*> relatab;

public:
  int size() const { return relatab.size(); }

  void addRelocation(RelocationTableEntry* relatabEntry) { relatab.push_back(relatabEntry); }
  RelocationTableEntry*& operator[](int index) { return relatab[index]; }
};

#endif // RELOCATION_TABLE_HPP_