#ifndef RELOCATION_TABLE_HPP_
#define RELOCATION_TABLE_HPP_

#include "types.h"
#include "defs.h"

class RelocationTable {
private:
  vector<RelocationTableEntry*> relatab;

public:
  int size() const { return relatab.size(); }
  
  bool notAssigned(RelocationTableEntry* relatabEntry) { 
    for (auto entry : relatab) {
      if (entry->offset == relatabEntry->offset) {
        return false;
      }
    }
    return true;
  }
  void addRelocation(RelocationTableEntry* relatabEntry) { relatab.push_back(relatabEntry); }
  RelocationTableEntry*& operator[](int index) { return relatab[index]; }
};

#endif // RELOCATION_TABLE_HPP_