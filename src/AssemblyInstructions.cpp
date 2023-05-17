#include "../inc/AssemblyInstructions.hpp"
#include "../inc/AssemblyException.hpp"
#include "../inc/SectionTable.hpp"
#include "../inc/Section.hpp"
#include "../inc/defs.h"
#include "../inc/utils.hpp"
#include <sstream>
#include <iomanip>

AssemblyInstructions& AssemblyInstructions::getInstance() {
  static AssemblyInstructions instance;
  return instance;
}

void AssemblyInstructions::firstPass() {
  SectionTable& sectiontab = SectionTable::getInstance();
  Section*& currentSection = sectiontab.getCurrentSection();
  if (currentSection == nullptr) {
    throw AssemblyException("Error: Instruction has to be defined inside section!");
  }
  
  currentSection->incrementLocationCounter(WORD_SIZE);
  currentSection->incrementSize(WORD_SIZE);
}

void AssemblyInstructions::halt() {
  Utils::addWord(SectionTable::getInstance().getCurrentSection(), 0);
}

void AssemblyInstructions::interrupt() {
  Utils::addWord(SectionTable::getInstance().getCurrentSection(), 0x10000000);
}

void AssemblyInstructions::xchg(string regOne, string regTwo) {
  int first = stoi(regOne.substr(1)), second = stoi(regTwo.substr(1));
  stringstream ss;

  ss << hex << 0x40;
  ss << hex << setw(2) << setfill('0') << first;
  ss << hex << setw(1) << second;
  ss << hex << setw(3) << 0;

  Utils::toBytesHex(ss);
}

void AssemblyInstructions::arithmetic(int modificator, string regOne, string regTwo) {
  int first = stoi(regOne.substr(1)), second = stoi(regTwo.substr(1));
  
  stringstream ss;
  ss << hex << setw(1) << 0x5 << modificator << second << first << second;
  ss << hex << setfill('0') << setw(3) << 0;

  Utils::toBytesHex(ss);
}

void AssemblyInstructions::logical(int modificator, string regOne, string regTwo) {
  int first = stoi(regOne.substr(1)), second = stoi(regTwo.substr(1));
  
  stringstream ss;
  ss << hex << setw(1) << 0x6 << modificator << second << first << second;
  ss << hex << setfill('0') << setw(3) << 0;

  Utils::toBytesHex(ss);
}