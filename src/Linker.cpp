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
  Assembler::getInstance().printSymbolTable();
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

void Linker::parseRelaEntry(string name, string line, int fileIndex) {
  SectionTable& sectiontab = SectionTable::getInstance();
  char delimiter = ' ';
  vector<string> entry;

  size_t startPos = 0, delimiterPos = line.find(delimiter);

  while (delimiterPos != string::npos) {
    string field = line.substr(startPos, delimiterPos - startPos);
    entry.push_back(field);
    startPos = delimiterPos + 1;
    delimiterPos = line.find(delimiter, startPos);
  }
  string last = line.substr(startPos);
  entry.push_back(last);

  relocations[name + to_string(fileIndex)].push_back(new RelocationTableEntry(
    stoi(entry[0], nullptr, 16),
    RELA_INFO(stoi(entry[2], nullptr, 16), stoi(entry[1], nullptr, 16)),
    stoi(entry[3], nullptr, 16)
  ));
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
  placeSections();
  // for (auto entry : outputSections) {
  //   cout << entry.first << ": " << entry.second << endl;
  // }
}