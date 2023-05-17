#ifndef RELOCATION_TABLE_HPP_
#define RELOCATION_TABLE_HPP_

#include "types.h"
#include "defs.h"

class RelocationTable {
private:
  struct RelocationTableEntry {
    ulong offset;
    ulong info;
    int addend;

    RelocationTableEntry(ulong _offset, ulong _info, int _addend = 0) {
      offset = _offset;
      info = _info;
      addend = _addend;
    } 
  };

  vector<RelocationTableEntry*> relatab;

public:
  int size() const { return relatab.size(); }

  RelocationTableEntry*& operator[](int index) { return relatab[index]; }
};

#endif // RELOCATION_TABLE_HPP_