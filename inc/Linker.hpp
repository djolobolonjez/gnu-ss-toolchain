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
  map<string, long> sectionsPlace;
  map<string, long> outputSections;
  map<string, long> inputSections;
  map<string, string> finalSections;
  map<string, vector<RelocationTableEntry*>> relocations;

  map<string, int> shdr;

  long locationCounter = 0;
  string output;

  vector<int> parseTableEntry(string, string*);
  void fixRelocations();
public:
  static Linker& getInstance();

  void setOutput(string output) { this->output = output; }

  void updateSectionHeaders(string, int, int, int);
  void placeSections();

  void fillSection(string, string);
  void parseRelaEntry(string, string, int);
  void populateSymbolTable(string, int);
  void checkSymbolTable();
  void resolveSymbols();
  void resolveRelocations();
  void checkIntersections();

  void parseInputFiles();
  void setInputFiles(vector<string> inputFiles) { this->inputFiles = inputFiles; }
  void setSectionsPlace(map<string, long> sectionsPlace) { this->sectionsPlace = sectionsPlace; }
};

#endif // LINKER_HPP_