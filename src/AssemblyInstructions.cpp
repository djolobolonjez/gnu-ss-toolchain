#include "../inc/AssemblyInstructions.hpp"
#include "../inc/AssemblyException.hpp"
#include "../inc/SectionTable.hpp"
#include "../inc/Section.hpp"
#include "../inc/defs.h"
#include "../inc/utils.hpp"
#include "../inc/SymbolTable.hpp"
#include "../inc/RelocationTable.hpp"
#include "../inc/Assembler.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>

AssemblyInstructions& AssemblyInstructions::getInstance() {
  static AssemblyInstructions instance;
  return instance;
}

void AssemblyInstructions::firstPass(InstructionArguments* instruction) {
  SectionTable& sectiontab = SectionTable::getInstance();
  Section*& currentSection = sectiontab.getCurrentSection();
  SymbolTable& symtab = SymbolTable::getInstance();
  if (currentSection == nullptr) {
    throw AssemblyException("Error: Instruction has to be defined inside section!");
  }
 
  if (instruction != nullptr) {
    ArgVector arguments = *(instruction->args);
    if (arguments[0].type == 0 && !symtab.foundEntryByName(*(arguments[0].value))) {
      symtab.addSymbol(*(arguments[0].value), 0, ST_INFO(STBIND_LOCAL, STTYPE_NOTYPE), currentSection->getId(), 0);
    }
  }
  
  currentSection->incrementLocationCounter(WORD_SIZE);
  currentSection->incrementSize(WORD_SIZE);
}

void AssemblyInstructions::literalPool(InstructionArguments* instruction) {
  ArgVector arguments = *(instruction->args);
  SectionTable& sectiontab = SectionTable::getInstance();
  Section*& currentSection = sectiontab.getCurrentSection();

  for (auto arg : arguments) {
    string literal = *(arg.value);
    map<string, int>& literalPool = currentSection->getPool();
    if (literalPool.find(literal) == literalPool.end() && arg.type != 2) {
      literalPool.insert({literal, currentSection->getPoolSize()});
      unsigned int data;

      if (arg.type == 1) {
        int base;
        if (arg.value->substr(0, 2) == "0x") {
          base = 16;
        }
        else {
          base = (arg.value->substr(0, 1) == "0" && arg.value->length() != 1 ? 8 : 10);
        }
        data = stoi(literal, nullptr, base);
      }
      else {
        SymbolTable& symtab = SymbolTable::getInstance();
        if (!symtab.foundEntryByName(literal)) {
          symtab.addSymbol(literal, 0, ST_INFO(STBIND_EXTERN, STTYPE_NOTYPE), UND, 0);
        }
        
        data = symtab[symtab.getIndexOfEntry(literal)]->getValue();
      }
      Utils::addWord(currentSection, data, false, 1);
  }
}
}

void AssemblyInstructions::halt() {
  stringstream ss; 
  ss << hex << setw(8) << HALT << setfill('0'); 
  Utils::toBytesHex(ss, true);
}

void AssemblyInstructions::interrupt() {
  stringstream ss;
  ss << hex << setw(8) << 0x10000000;
  Utils::toBytesHex(ss, true);
  //Utils::addWord(SectionTable::getInstance().getCurrentSection(), INT, true);
}

void AssemblyInstructions::iret() {
  stringstream ss;
  ss << hex << setw(1) << (LD | 0x6);
  ss << hex << setw(1) << 0x0 << 0xe << 0x0 << setfill('0') << setw(3) << 0x4;
  Utils::toBytesHex(ss, true);

  ss.str("");
  ss.clear();
  ss << hex << setw(1) << (LD | 0x3);
  ss << hex << setw(1) << 0xf << 0xe << 0x0 << setfill('0') << setw(3) << 0x8;
  Utils::toBytesHex(ss, true);
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
  ss << hex << setw(1) << (ARITHMETIC | modificator) << second << first << second;
  ss << hex << setfill('0') << setw(3) << 0;

  Utils::toBytesHex(ss, true);
}

void AssemblyInstructions::logical(int modificator, string regOne, string regTwo) {
  int first = stoi(regOne.substr(1)), second = stoi(regTwo.substr(1));
  
  stringstream ss;
  ss << hex << setw(1) << (LOGICAL | modificator) << second << first << second;
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
      code.third = stoi(arguments[0].value->substr(1));

      ss << hex << setw(1) << code.first << code.second << code.third << setfill('0') << setw(3) << code.displacement;
      break;
    }
    case 0x2: {
      code.third = stoi(arguments[1].value->substr(1));
      Section*& currentSection = SectionTable::getInstance().getCurrentSection();
      if (arguments[0].type == 1 || arguments[0].type == 0) {
        code.second = 15;
        
        map<string, int>& literalPool = currentSection->getPool();
        code.displacement = literalPool[*(arguments[0].value)] - currentSection->getLocationCounter() - 4;
        if (code.displacement >= MAX_DISPLACEMENT) {
          throw AssemblyException("Error: Maximum displacement exceeded!");
        }
      }
      else {
        code.first = stoi(arguments[0].value->substr(1));
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
            locationCounter + code.displacement + 4, RELA_INFO(stEntry->getIndex(), R_X86_64_32), "LOC", stEntry->getValue()
          );
        }
        else if ((ST_BIND(stEntry->getInfo()) == STBIND_GLOBAL) || (stEntry->isDefined() == false)) {
          relaEntry = new RelocationTableEntry(locationCounter + code.displacement + 4, RELA_INFO(symbolIndex, R_X86_64_32), "GLOB");
        }

        if (relaEntry != nullptr && relatab->notAssigned(relaEntry)) {
          relatab->addRelocation(relaEntry);
        }
      }
      ss << hex << setw(1) << code.first << code.second << code.third << setfill('0') << setw(3) << code.displacement;
      break;
    }
    case 0x3 : {
      swap(arguments[0], arguments[1]);
      instruction->modificator = 0x1;
      ld(instruction, {});
      return;
      break;
    }
    case 0x1: {
      if (code.first == 0) {
        code.first = stoi(arguments[0].value->substr(1));
      }
      ss << hex << setw(1) << code.first << setw(1) << code.second << setw(1) << code.third << setfill('0') << setw(3) << (code.displacement & 0xfff);
      break;
    }
    case 0x8: {
      code.third = stoi(arguments[1].value->substr(1));
      code.first = (code.third == 0xd ? 0x1 : 0xd);

      InstructionArguments* registerBackup = Utils::create_instruction(0x1);
      string* newRegister = new string("%" + to_string(code.first));
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
      st(registerBackup, {});

      firstPass();

      registerBackup->modificator = 0x3;
      registerBackup->args->clear();
      registerBackup->args->push_back({newRegister, 2});
      pop(registerBackup);

      return;

      break;
    } 

    case 0x9: {
      code.third = stoi(arguments[2].value->substr(1));
      code.first = stoi(arguments[0].value->substr(1));
      cout << "OK" << endl;
      if (code.third != 0xd && code.first != 0xd) {
        code.second = 0xd;
      }
      else {
        if (code.first != 0x1 && code.third != 0x1) {
          code.second = 0x1;
        }
        else {
          code.second = 0x2;
        }
      }
      InstructionArguments* registerBackup = Utils::create_instruction(0x1);
      string* newRegister = new string("%" + to_string(code.second));
      registerBackup->args->push_back({newRegister, 2});
      push(registerBackup);

      firstPass();
      
      registerBackup->modificator = 0x2;
      registerBackup->args->clear();
      registerBackup->args->push_back({arguments[1].value, arguments[1].type});
      registerBackup->args->push_back({newRegister, 0x2});
      ld(registerBackup, {});

      firstPass();

      registerBackup->args->clear();
      registerBackup->args->push_back({arguments[0].value, 0x2});
      registerBackup->args->push_back({arguments[2].value, 0x2});
      st(registerBackup, {0, code.second, 0, 0});

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

void AssemblyInstructions::ld(InstructionArguments* instruction, opcode code) {
  ArgVector arguments = *(instruction->args);
  int mod = instruction->modificator;
  Section*& currentSection = SectionTable::getInstance().getCurrentSection();
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
      
      if (arguments[0].type == 1 || arguments[0].type == 0) {
        code.second = 0xf;
        
        map<string, int>& literalPool = currentSection->getPool();
        code.displacement = literalPool[*(arguments[0].value)] - currentSection->getLocationCounter() - 4;
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
            locationCounter + code.displacement + 4, RELA_INFO(stEntry->getIndex(), R_X86_64_32), "LOC", stEntry->getValue()
          );
        }
        else if ((ST_BIND(stEntry->getInfo()) == STBIND_GLOBAL) || (stEntry->isDefined() == false)) {
          relaEntry = new RelocationTableEntry(locationCounter + code.displacement + 4, RELA_INFO(symbolIndex, R_X86_64_32), "GLOB");
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
      string* newRegister = new string("%" + to_string(code.second));
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

    case 0x9: {
      code.first = stoi(arguments[2].value->substr(1));
      code.second = stoi(arguments[0].value->substr(1));
      if (code.first != 0xd && code.second != 0xd) {
        code.third = 0xd;
      }
      else {
        if (code.first != 0x1 && code.second != 0x1) {
          code.third = 0x1;
        }
        else {
          code.third = 0x2;
        }
      }
      InstructionArguments* registerBackup = Utils::create_instruction(0x1);
      string* newRegister = new string("%" + to_string(code.third));
      registerBackup->args->push_back({newRegister, 2});
      push(registerBackup);

      firstPass();
      
      registerBackup->modificator = 0x2;
      registerBackup->args->clear();
      registerBackup->args->push_back({arguments[1].value, arguments[1].type});
      registerBackup->args->push_back({newRegister, 0x2});
      ld(registerBackup, {});

      firstPass();

      registerBackup->args->clear();
      registerBackup->args->push_back({arguments[0].value, 0x2});
      registerBackup->args->push_back({arguments[2].value, 0x2});
      ld(registerBackup, {0, 0, code.third, 0});

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

void AssemblyInstructions::call(InstructionArguments* instruction) {
  ArgVector arguments = *(instruction->args);
  int mod = instruction->modificator;
  Section*& currentSection = SectionTable::getInstance().getCurrentSection();
  
  stringstream ss;
  ss << hex << setw(1) << (CALL | mod);

  int displacement = 0;
        
  map<string, int>& literalPool = currentSection->getPool();
  displacement = literalPool[*(arguments[0].value)] - currentSection->getLocationCounter() - 4;
  if (displacement > MAX_DISPLACEMENT) {
    throw AssemblyException("Error: Maximum displacement exceeded!");
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
        locationCounter + displacement + 4, RELA_INFO(stEntry->getIndex(), R_X86_64_32), "LOC", stEntry->getValue()
      );
    }
    else if ((ST_BIND(stEntry->getInfo()) == STBIND_GLOBAL) || (stEntry->isDefined() == false)) {
      relaEntry = new RelocationTableEntry(locationCounter + displacement + 4, RELA_INFO(symbolIndex, R_X86_64_32), "GLOB");
    }

    if (relaEntry != nullptr) {
      relatab->addRelocation(relaEntry);
    }
  }

  ss << hex << setw(1) << 0xf << 0x0 << 0x0 << setfill('0') << setw(3) << displacement;
  Utils::toBytesHex(ss, true);
}

void AssemblyInstructions::jmp(InstructionArguments* instruction) {
  ArgVector arguments = *(instruction->args);
  int mod = instruction->modificator;
  Section*& currentSection = SectionTable::getInstance().getCurrentSection();
  
  stringstream ss;
  ss << hex << setw(1) << (JMP | mod);

  int first = 0xf, second = 0x0, third = 0x0;
  if (arguments.size() > 1) {
    second = stoi(arguments[1].value->substr(1));
    third = stoi(arguments[2].value->substr(1));
  }

  int displacement = 0;
  int type = arguments[0].type;
  string literal = *(arguments[0].value);

  map<string, int>& literalPool = currentSection->getPool();
  displacement = literalPool[literal] - currentSection->getLocationCounter() - 4;
  if (displacement > MAX_DISPLACEMENT) {
    throw AssemblyException("Error: Maximum displacement exceeded!");
  }

  if (type == 0) {
    RelocationTable*& relatab = currentSection->getRelaLink();
    int locationCounter = currentSection->getLocationCounter();
    RelocationTableEntry* relaEntry = nullptr;

    SymbolTable& symtab = SymbolTable::getInstance();
    int symbolIndex = symtab.getIndexOfEntry(*(arguments[0].value));
    SymbolTableEntry*& stEntry = symtab[symbolIndex];

    if (ST_BIND(stEntry->getInfo()) == STBIND_LOCAL) {
      relaEntry = new RelocationTableEntry(
        locationCounter + displacement + 4, RELA_INFO(stEntry->getIndex(), R_X86_64_32), "LOC", stEntry->getValue()
      );
    }
    else if ((ST_BIND(stEntry->getInfo()) == STBIND_GLOBAL) || (stEntry->isDefined() == false)) {
      relaEntry = new RelocationTableEntry(locationCounter + displacement + 4, RELA_INFO(symbolIndex, R_X86_64_32), "GLOB");
    }

    if (relaEntry != nullptr) {
      relatab->addRelocation(relaEntry);
    }
  }

  ss << hex << setw(1) << first << second << third << setfill('0') << setw(3) << displacement;
  Utils::toBytesHex(ss, true);
}

void AssemblyInstructions::csrrd(InstructionArguments* instruction) {
  ArgVector arguments = *(instruction->args);
  int mod = instruction->modificator;
  int first = stoi(arguments[1].value->substr(1));
  int second = 0;

  string csr = *(arguments[0].value);
  if (csr == "handler") {
    second = 1;
  }
  else if (csr == "cause") {
    second = 2;
  }

  stringstream ss;
  ss << hex << setw(1) << (LD | mod);
  ss << hex << setw(1) << first << second << 0x0 << setfill('0') << setw(3) << 0x0;

  Utils::toBytesHex(ss, true);
}