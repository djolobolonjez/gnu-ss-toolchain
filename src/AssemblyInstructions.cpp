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
  Utils::addWord(SectionTable::getInstance().getCurrentSection(), HALT, true);
}

void AssemblyInstructions::interrupt() {
  Utils::addWord(SectionTable::getInstance().getCurrentSection(), INT, true);
}

void AssemblyInstructions::xchg(string regOne, string regTwo) {
  int first = stoi(regOne.substr(1)), second = stoi(regTwo.substr(1));
  stringstream ss;

  ss << hex << XCHG;
  ss << hex << setw(2) << setfill('0') << first;
  ss << hex << setw(1) << second;
  ss << hex << setw(3) << 0;

  Utils::toBytesHex(ss, true);
}

void AssemblyInstructions::arithmetic(int modificator, string regOne, string regTwo) {
  int first = stoi(regOne.substr(1)), second = stoi(regTwo.substr(1));
  
  stringstream ss;
  ss << hex << setw(1) << ARITHMETIC << modificator << second << first << second;
  ss << hex << setfill('0') << setw(3) << 0;

  Utils::toBytesHex(ss, true);
}

void AssemblyInstructions::logical(int modificator, string regOne, string regTwo) {
  int first = stoi(regOne.substr(1)), second = stoi(regTwo.substr(1));
  
  stringstream ss;
  ss << hex << setw(1) << LOGICAL << modificator << second << first << second;
  ss << hex << setfill('0') << setw(3) << 0;

  Utils::toBytesHex(ss, true);
}

void AssemblyInstructions::shl(string regOne, string regTwo) {
  int first = stoi(regOne.substr(1)), second = stoi(regTwo.substr(1));

  stringstream ss;
  ss << hex << setw(1) << SHL << first << first << second;
  ss << hex << setfill('0') << setw(3) << 0;
  Utils::toBytesHex(ss, true);
}

void AssemblyInstructions::shr(string regOne, string regTwo) {
  int first = stoi(regOne.substr(1)), second = stoi(regTwo.substr(1));

  stringstream ss;
  ss << hex << setw(1) << SHR << first << first << second;
  ss << hex << setfill('0') << setw(3) << 0;
  Utils::toBytesHex(ss, true);
}

void AssemblyInstructions::ld(InstructionArguments* instruction, opcode code) {
  ArgVector arguments = *(instruction->args);
  int mod = instruction->modificator;

  stringstream ss;
  ss << hex << setw(1) << (LD | mod);

  switch (mod) {
    case 0x1: {
      code.first = stoi(arguments[1].value->substr(1));
      code.second = stoi(arguments[0].value->substr(1));
      
      ss << hex << setw(1) << code.first << code.second << code.third << setfill('0') << setw(3) << code.displacement;
      break;
    }

    case 0x2: {
      code.first = stoi(arguments[1].value->substr(1));
      code.second = stoi(arguments[0].value->substr(1));
      ss << hex << setw(1) << code.first << code.second << code.third << setfill('0') << setw(3) << code.displacement;
      break;
    }

    case 0x3 : {
      ss << hex << setw(1) << code.first << code.second << code.third << setfill('0') << setw(3) << code.displacement;
      break;
      //mozda ce i ovo biti suvisno
    }

    default: {
      throw AssemblyException("Error: Illegal instruction modificator!");
    }
  }

  Utils::toBytesHex(ss, true);
}

void AssemblyInstructions::push(InstructionArguments* instruction) {

}

void AssemblyInstructions::pop(InstructionArguments* instruction) {
  ld(instruction, {0, 14, 0, 4});
}