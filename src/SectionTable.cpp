#include "../inc/SectionTable.hpp"
#include "../inc/Section.hpp"
#include "../inc/SymbolTable.hpp"
#include "../inc/defs.h"

SectionTable::SectionTable() { }

SectionTable& SectionTable::getInstance() {
  static SectionTable instance;
  return instance;
}

void SectionTable::addSection(string name) {
  SymbolTable& symtab = SymbolTable::getInstance();
  int sectionID = symtab.getSymbolID();
  symtab.addSymbol(name, 0, ST_INFO(STBIND_LOCAL, STTYPE_SECTION), sectionID, 0);
  sectionIndexMap[name] = size();
  sectiontab.push_back(new Section(name, sectionID));
}