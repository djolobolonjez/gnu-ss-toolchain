#ifndef SECTION_TABLE_HPP_
#define SECTION_TABLE_HPP_

#include "types.h"
#include <map>

class Section;

class SectionTable {
private:
  SectionTable();

  vector<Section*> sectiontab;
  map<string, int> sectionIndexMap;
  Section* currentSection = nullptr;

public:
  static SectionTable& getInstance();

  Section*& operator[](int index) { return sectiontab[index]; }
  
  int size() const { return sectiontab.size(); }

  Section*& getCurrentSection() { return currentSection; }
  void setSection(Section* section) { currentSection = section; }

  void addSection(string name);

  bool foundSectionByName(string name) { return sectionIndexMap.find(name) != sectionIndexMap.end(); }

  int getSectionIndexByName(string name) { return sectionIndexMap[name]; }
};

#endif // SECTION_TABLE_HPP_