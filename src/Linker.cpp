#include "../inc/Linker.hpp"
#include "../inc/LinkerException.hpp"
#include "../inc/SectionTable.hpp"
#include "../inc/Section.hpp"
#include "../inc/utils.hpp"
#include "../inc/types.h"
#include "../inc/defs.h"
#include "../inc/SymbolTable.hpp"
#include "../inc/Assembler.hpp"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iomanip>

Linker& Linker::getInstance() {
  static Linker instance;
  return instance;
}

void Linker::updateSectionHeaders(string name, int size, int fileIndex, int index) {
  if (outputSections.find(name) == outputSections.end()) {
    outputSections[name] = 0;
  }
  inputSections[name + to_string(fileIndex)] = outputSections[name];
  outputSections[name] += size;
  finalSections[name + to_string(fileIndex)] = name;
  fileSectionMap[fileIndex][index] = name;
}

void Linker::placeSections() {
  for (auto entry : outputSections) {
    inputSections[entry.first] = outputSections[entry.first];
    if (sectionsPlace.find(entry.first) != sectionsPlace.end()) {
      outputSections[entry.first] = sectionsPlace[entry.first];
    }
    else {
      int size = entry.second;
      outputSections[entry.first] = locationCounter;
      locationCounter += size;
    }
  }
  SymbolTable& symtab = SymbolTable::getInstance();
  for (auto entry : outputSections) {
    int symbolIndex = symtab.getIndexOfEntry(entry.first);
    symtab[symbolIndex]->setValue(entry.second);
  }
}

void Linker::fillSection(string name, string line) {
  SectionTable& sectiontab = SectionTable::getInstance();
  if (!sectiontab.foundSectionByName(name)) {
    sectiontab.addSection(name);
  }

  Section*& section = sectiontab[sectiontab.getSectionIndexByName(name)];

  for (int i = 0; i < line.length(); i += 8) {
    string hexByte = line.substr(i, 8);
    istringstream iss(hexByte);
    unsigned int value;

    iss >> std::hex >> value;
    Utils::addWord(section, value, true);
  }

  // vector<char>& v = section->getContent();
  // for (int address = 0; address < v.size(); address += 8) {
  //   std::cout << setw(4) << setfill('0') << hex << address << ": ";

  //   for (int offset = 0; offset < 8; offset++) {
  //       int byteAddress = address + offset;
  //       if (byteAddress < v.size())
  //           std::cout << setw(2) << setfill('0') << hex << ((int)(unsigned char)v[byteAddress] & 0xff);
  //       else
  //           std::cout << "  ";

  //       std::cout << " ";
  //   }

  //   std::cout << std::endl;
  // } 
  // cout << endl;
}

void Linker::checkSymbolTable() {
  SymbolTable& symtab = SymbolTable::getInstance();
  for (int i = 1; i < symtab.size(); i++) {
    if (symtab[i]->getIndex() == UND) {
      throw LinkerException("Undefined reference to symbol " + symtab[i]->getName());
    }
  }
}

vector<int> Linker::parseTableEntry(string line, string* name = nullptr) {
  char delimiter = ' ';
  vector<int> entry;

  size_t startPos = 0, delimiterPos = line.find(delimiter);

  while (delimiterPos != string::npos) {
    string field = line.substr(startPos, delimiterPos - startPos);
    entry.push_back(stoi(field, nullptr, 16));
    startPos = delimiterPos + 1;
    delimiterPos = line.find(delimiter, startPos);
  }
  string last = line.substr(startPos);
  if (name == nullptr) {
    entry.push_back(stoi(last, nullptr, 16));
  }
  else {
    *name = last;
  }
  
  return entry;
}

void Linker::parseRelaEntry(string name, string line, int fileIndex) {
  SectionTable& sectiontab = SectionTable::getInstance();
  string type;
  vector<int> entry = parseTableEntry(line, &type);

  relocations[name + to_string(fileIndex)].push_back(new RelocationTableEntry(
    entry[0],
    RELA_INFO(entry[2], entry[1]),
    type,
    entry[3]
  ));
}

void Linker::fixRelocation(vector<RelocationTableEntry*>& relocationEntries, int index, int oldIndex, string type) {
  for (auto rela : relocationEntries) {
    if (oldIndex == RELA_SYM(rela->info) && rela->type == type) {
      ulong info = rela->info;
      rela->info = RELA_INFO(index, RELA_TYPE(info));
    }
    
  }
}

void Linker::populateSymbolTable(string line, int fileIndex) {
  SymbolTable& symtab = SymbolTable::getInstance();
  string name;
  vector<int> entry = parseTableEntry(line, &name);

  string section = fileSectionMap[fileIndex][entry[4]];
  string childSection = section + to_string(fileIndex);
  int index = symtab.getIndexOfEntry(section);

  vector<RelocationTableEntry*> relaEntries = relocations[childSection];
  int newIndex = 0;

  if (entry[2] == STTYPE_SECTION) {
    return;
  }
  if (entry[3] == STBIND_LOCAL) {
    name += "(" + to_string(fileIndex) + ")";
    int sectionIndex = SectionTable::getInstance().getSectionIndexByName(section);
    newIndex = SectionTable::getInstance()[sectionIndex]->getId();
    fixRelocation(relaEntries, newIndex, shdr[section], "LOC");
  }
  else if (entry[3] == STBIND_GLOBAL){
    newIndex = symtab.size();
    fixRelocation(relaEntries, newIndex, entry[0], "GLOB");
    if (symtab.foundEntryByName(name)) {
      int symbolIndex = symtab.getIndexOfEntry(name);
      if (ST_BIND(symtab[symbolIndex]->getInfo()) == STBIND_GLOBAL) {
        throw LinkerException("Symbol " + name + " is already defined!");
      }
      else {
        if (ST_BIND(symtab[symbolIndex]->getInfo()) == STBIND_EXTERN) {
          symtab[symbolIndex]->setDefined(true);
          symtab[symbolIndex]->setIndex(index);
          symtab[symbolIndex]->setValue(entry[1]);
          return;
        }
      }
    }
  }
  else {
    newIndex = symtab.size();
    fixRelocation(relaEntries, newIndex, entry[0], "GLOB");
    if (symtab.foundEntryByName(name)) {
      return;
    }
  }
  symtab.addSymbol(name, entry[1] + inputSections[childSection], ST_INFO(entry[3], entry[2]), index, 0);
}

void Linker::resolveSymbols() {
  SymbolTable& symtab = SymbolTable::getInstance();
  for (int i = 0; i < symtab.size(); i++) {
    if (ST_TYPE(symtab[i]->getInfo()) != STTYPE_SECTION) {
      int index = symtab[i]->getIndex();
      string section = symtab[index]->getName();
      long offset = outputSections[section];
      long value = symtab[i]->getValue();
      symtab[i]->setValue(value + offset);
    }
  }

  Assembler::getInstance().printSymbolTable();
}

void Linker::resolveRelocations() {
  SymbolTable& symtab = SymbolTable::getInstance();
  SectionTable& sectiontab = SectionTable::getInstance();

  for (auto entry : relocations) {
    string childSection = entry.first;
    vector<RelocationTableEntry*> relocationEntries = entry.second;

    for (auto rela : relocationEntries) {
      ulong value = rela->addend + symtab[RELA_SYM(rela->info)]->getValue();
      string section = finalSections[childSection];
      int sectionIndex = sectiontab.getSectionIndexByName(section);

      vector<char>& content = sectiontab[sectionIndex]->getContent();
      int offset = rela->offset + inputSections[childSection];

      stringstream ss; 
      ss << setw(8) << setfill('0') << hex << value;
      unsigned data;
      ss >> hex >> data;
      Utils::editContent(content, data, false, offset);
    }
  }
}

void Linker::checkIntersections() {
  for (auto lhs : outputSections) {
    for (auto rhs : outputSections) {
      int lhsLower = lhs.second;
      int lhsUpper = lhsLower + inputSections[lhs.first];
      int rhsLower = rhs.second;
      int rhsUpper = rhsLower + inputSections[rhs.first];
      if (lhs.first != rhs.first && (max(lhsLower, rhsLower) < min(lhsUpper, rhsUpper))) {
        throw LinkerException("Sections "  + lhs.first + " and " + rhs.first + " overlap!");
      }
    }
  }
}

void Linker::parseInputFiles() {
  if (inputFiles.size() == 0) {
    throw LinkerException("Linker error: Nothing to be linked!");
  }

  int fileIndex = 1;
  for (string filename : inputFiles) {
    ifstream inputFile(filename);
    if (!inputFile.is_open()) {
      throw LinkerException("Error opening file!");
    }
    string inputLine;
    vector<string> fileContent;
    while (getline(inputFile, inputLine)) {
      fileContent.push_back(inputLine);
    }
    
    int cursor = 0, fileSize = fileContent.size();
    while (cursor < fileSize) {
      if (fileContent[cursor] == "#shdr") {
        cursor++;
        while (fileContent[cursor] != "#") {
          int sep = fileContent[cursor].find(" ");
          string name = fileContent[cursor].substr(0, sep);
          int current = sep;
          sep = fileContent[cursor].find(" ", current + 1);
          int size = stoi(fileContent[cursor].substr(current + 1, sep - current - 1));
          int index = stoi(fileContent[cursor].substr(sep + 1));
          updateSectionHeaders(name, size, fileIndex, index);
          shdr[name] = index;
          cursor++;
        }
      }
      else if (fileContent[cursor] == "---"){
        cursor++;
        string name = fileContent[cursor++];
        while (fileContent[cursor] != "#") {
          string line = fileContent[cursor++];
          if (line != "\n") {
            auto remove = [](char c) {
              return c == ' ';
            };
            line.erase(std::remove_if(line.begin(), line.end(), remove), line.end());
            fillSection(name, line);
          }
        }
      }
      else if (fileContent[cursor].substr(0, 5) == ".rela") {
        string name = fileContent[cursor++].substr(6);
        while (fileContent[cursor] != "#") {
          string line = fileContent[cursor++];
          parseRelaEntry(name, line.substr(2), fileIndex);
        }
      }
      else if (fileContent[cursor++] == "#.symtab") {
        while (fileContent[cursor] != "#") {
          string line = fileContent[cursor++];
          populateSymbolTable(line.substr(2), fileIndex);
        }
      }
    }
    inputFile.close();
    fileIndex++;
  }
  // for (auto entry : relocations) {
  //   cout << entry.first << endl;
  //   for (auto rela : entry.second) {
  //     cout << hex << rela->offset << "\t";
  //     if (RELA_TYPE(rela->info) == R_X86_64_32) {
  //       cout << "R_X86_64_32";
  //     }
  //     cout << "\t";
  //     cout << hex << RELA_SYM(rela->info) << "\t";
  //     cout << hex << rela->addend << endl;
  //   }
  // }
  checkSymbolTable();
  placeSections();
  checkIntersections();
  resolveSymbols();
  resolveRelocations();

  ofstream outputFile(output);

  if (!outputFile.is_open()) {
    throw LinkerException("Unable to generate output file!");
  }

  SectionTable& sectiontab = SectionTable::getInstance();
  for (int i = 0; i < sectiontab.size(); i++) {
    vector<char>& v = sectiontab[i]->getContent();
    string name = sectiontab[i]->getName();
    for (int address = 0; address < v.size(); address += 4) {
      outputFile << setw(8) << setfill('0') << hex << address + outputSections[name] << ": ";

      for (int offset = 0; offset < 4; offset++) {
          int byteAddress = address + offset;
          if (byteAddress < v.size())
              outputFile << setw(2) << setfill('0') << hex << ((int)(unsigned char)v[byteAddress] & 0xff);
          else
              outputFile << " ";

          
      }

      outputFile << "\n";
    } 
  }
  outputFile.close();
  
  // for (auto entry : outputSections) {
  //   cout << entry.first << ": " << entry.second << endl;
  // }
}