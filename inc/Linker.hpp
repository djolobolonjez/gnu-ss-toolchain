#ifndef LINKER_HPP_
#define LINKER_HPP_

#include <vector>
#include <map>
#include <iostream>

using namespace std;

struct RelocationTableEntry;

class Linker {
private:
  Linker() = default;

  vector<string> inputFiles;
  map<int, map<int, string>> fileSectionMap;
  map<string, int> sectionsPlace;
  map<string, int> outputSections;
  map<string, int> inputSections;
  map<string, string> finalSections;
  map<string, vector<RelocationTableEntry*>> relocations;

  int locationCounter = 0;
public:
  static Linker& getInstance();

  void updateSectionHeaders(string, int, int, int);
  void placeSections();

  void fillSection(string, string);
  void parseRelaEntry(string, string, int);
  
  void parseInputFiles();
  void setInputFiles(vector<string> inputFiles) { this->inputFiles = inputFiles; }
  void setSectionsPlace(map<string, int> sectionsPlace) { this->sectionsPlace = sectionsPlace; }
};

#endif // LINKER_HPP_