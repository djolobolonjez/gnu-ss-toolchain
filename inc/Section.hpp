#ifndef SECTION_HPP_
#define SECTION_HPP_

#include "types.h"

class RelocationTable;

class Section {
private:
  string name;
  
  int id;
  int locationCounter;
  int size;
  
  RelocationTable* relaLink; // add the implementation of rela table entry
  
  ulong vaddr;
  ulong offset;
  
  vector<char> content;
public:
  Section(string _name, int _id);

  string getName() const { return name; }
  int getId() const { return id; }
  int getLocationCounter() const { return locationCounter; }
  int getSize() const { return size; }
  RelocationTable* getRelaLink() const { return relaLink; }
};

#endif // SECTION_HPP_