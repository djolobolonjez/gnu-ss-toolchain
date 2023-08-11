#include "../inc/AssemblyInstructions.hpp"
#include "../inc/AssemblyException.hpp"
#include "../inc/SectionTable.hpp"
#include "../inc/Section.hpp"
#include "../inc/defs.h"
#include "../inc/utils.hpp"
#include "../inc/SymbolTable.hpp"
#include "../inc/RelocationTable.hpp"
#include "../inc/Assembler.hpp"
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

void AssemblyInstructions::literalPool(InstructionArguments* instruction) {
  ArgVector arguments = *(instruction->args);
  SectionTable& sectiontab = SectionTable::getInstance();
  Section*& currentSection = sectiontab.getCurrentSection();

  string literal = *(arguments[0].value);
  map<string, int>& literalPool = currentSection->getPool();
  if (literalPool.find(literal) == literalPool.end()) {
    literalPool.insert({literal, currentSection->getPoolSize()});
    unsigned int data;

    if (arguments[0].type == 1) {
      data = stoi(literal);
    }
    else {
      SymbolTable& symtab = SymbolTable::getInstance();
      data = symtab[symtab.getIndexOfEntry(literal)]->getValue();
    }
    Utils::addWord(currentSection, data, false, 1);

  }
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

void AssemblyInstructions::st(InstructionArguments* instruction, opcode code) {
  ArgVector arguments = *(instruction->args);
  int mod = instruction->modificator;

  stringstream ss;
  ss << hex << setw(1) << (ST | mod);

  switch (mod) {
    case 0x0: {
      code.first = stoi(arguments[1].value->substr(1));
      code.second = stoi(arguments[0].value->substr(1));

      ss << hex << setw(1) << code.first << code.second << code.third << setfill('0') << setw(3) << code.displacement;
      break;
    }
    case 0x2: {
      code.first = stoi(arguments[1].value->substr(1));
      Section*& currentSection = SectionTable::getInstance().getCurrentSection();
      if (arguments[0].type == 1 || arguments[0].type == 0) {
        code.second = 15;
        
        map<string, int>& literalPool = currentSection->getPool();
        code.displacement = literalPool[*(arguments[0].value)] - currentSection->getLocationCounter();
        if (code.displacement >= MAX_DISPLACEMENT) {
          throw AssemblyException("Error: Maximum displacement exceeded!");
        }
      }

      if (arguments[0].type == 0) {
        RelocationTable*& relatab = currentSection->getRelaLink();
        int locationCounter = currentSection->getLocationCounter();
        RelocationTableEntry* relaEntry = nullptr;

        SymbolTable& symtab = SymbolTable::getInstance();
        int symbolIndex = symtab.getIndexOfEntry(*(arguments[0].value));
        SymbolTableEntry*& stEntry = symtab[symbolIndex];

        if (ST_BIND(stEntry->getInfo()) == STBIND_LOCAL) {
          relaEntry = new RelocationTableEntry(
            locationCounter + code.displacement, RELA_INFO(stEntry->getIndex(), R_X86_64_32), stEntry->getValue()
          );
        }
        else if ((ST_BIND(stEntry->getInfo()) == STBIND_GLOBAL) || (stEntry->isDefined() == false)) {
          relaEntry = new RelocationTableEntry(locationCounter + code.displacement, RELA_INFO(symbolIndex, R_X86_64_32));
        }

        if (relaEntry != nullptr && relatab->notAssigned(relaEntry)) {
          relatab->addRelocation(relaEntry);
        }
      }
      ss << hex << setw(1) << code.first << code.second << code.third << setfill('0') << setw(3) << code.displacement;
      break;
    }
    case 0x3 : {
      if (code.first == 0) {
        code.first = stoi(arguments[0].value->substr(1));
      }
      
      ss << hex << setw(1) << code.first << code.second << code.third << setfill('0') << setw(3) << code.displacement;
      break;
    }
    case 0x1: {
      if (code.first == 0) {
        code.first = stoi(arguments[0].value->substr(1));
      }
      cout << code.first << endl;
      ss << hex << setw(1) << code.first << setw(1) << code.second << setw(1) << code.third << setfill('0') << setw(3) << (code.displacement & 0xfff);
      break;
    }
  }
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
      Section*& currentSection = SectionTable::getInstance().getCurrentSection();
      if (arguments[0].type == 1 || arguments[0].type == 0) {
        code.second = 0xf;
        
        map<string, int>& literalPool = currentSection->getPool();
        code.displacement = literalPool[*(arguments[0].value)] - currentSection->getLocationCounter();
        if (code.displacement > MAX_DISPLACEMENT) {
          throw AssemblyException("Error: Maximum displacement exceeded!");
        }
      }
      else {
        code.second = stoi(arguments[0].value->substr(1));
      }

      if (arguments[0].type == 0) {
        RelocationTable*& relatab = currentSection->getRelaLink();
        int locationCounter = currentSection->getLocationCounter();
        RelocationTableEntry* relaEntry = nullptr;

        SymbolTable& symtab = SymbolTable::getInstance();
        int symbolIndex = symtab.getIndexOfEntry(*(arguments[0].value));
        SymbolTableEntry*& stEntry = symtab[symbolIndex];

        if (ST_BIND(stEntry->getInfo()) == STBIND_LOCAL) {
          relaEntry = new RelocationTableEntry(
            locationCounter + code.displacement, RELA_INFO(stEntry->getIndex(), R_X86_64_32), stEntry->getValue()
          );
        }
        else if ((ST_BIND(stEntry->getInfo()) == STBIND_GLOBAL) || (stEntry->isDefined() == false)) {
          relaEntry = new RelocationTableEntry(locationCounter + code.displacement, RELA_INFO(symbolIndex, R_X86_64_32));
        }

        if (relaEntry != nullptr) {
          relatab->addRelocation(relaEntry);
        }
      }
      ss << hex << setw(1) << code.first << code.second << code.third << setfill('0') << setw(3) << code.displacement;
      break;
    }
    case 0x3: {
      if (code.first == 0) {
        code.first = stoi(arguments[0].value->substr(1));
      }
      
      ss << hex << setw(1) << code.first << code.second << code.third << setfill('0') << setw(3) << code.displacement;
      break;
    }
    case 0x8: {
      code.first = stoi(arguments[1].value->substr(1));
      code.second = (code.first == 0xd ? 0x1 : 0xd);

      InstructionArguments* registerBackup = Utils::create_instruction(0x1);
      cout << code.second << endl;
      string* newRegister = new string("%" + to_string(code.second));
      cout << *newRegister << endl;
      registerBackup->args->push_back({newRegister, 2});
      push(registerBackup);

      firstPass();
      
      registerBackup->modificator = 0x2;
      registerBackup->args->clear();
      registerBackup->args->push_back({arguments[0].value, arguments[0].type});
      registerBackup->args->push_back({newRegister, 0x2});
      ld(registerBackup, {});

      firstPass();

      registerBackup->args->clear();
      registerBackup->args->push_back({newRegister, 0x2});
      registerBackup->args->push_back({arguments[1].value, 0x2});
      ld(registerBackup, {});

      firstPass();

      registerBackup->modificator = 0x3;
      registerBackup->args->clear();
      registerBackup->args->push_back({newRegister, 2});
      pop(registerBackup);

      return;

      break;
    } 

  }

  Utils::toBytesHex(ss, true);
}

void AssemblyInstructions::ret(InstructionArguments* instruction) {
  ld(instruction, {15, 14, 0, 4});
}

void AssemblyInstructions::push(InstructionArguments* instruction) {
  st(instruction, {0, 14, 0, -4});
}

void AssemblyInstructions::pop(InstructionArguments* instruction) {
  ld(instruction, {0, 14, 0, 4});
}