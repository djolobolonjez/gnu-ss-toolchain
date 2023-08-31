#include "../inc/AssemblyDirectives.hpp"
#include "../inc/SymbolTable.hpp"
#include "../inc/defs.h"
#include "../inc/SectionTable.hpp"
#include "../inc/Section.hpp"
#include "../inc/AssemblyException.hpp"
#include "../inc/utils.hpp"
#include "../inc/RelocationTable.hpp"
#include "../inc/Assembler.hpp"
#include <iomanip>
#include <fstream>

AssemblyDirectives& AssemblyDirectives::getInstance() {
  static AssemblyDirectives instance;
  return instance;
}

void AssemblyDirectives::checkNoSection() {
  Section*& currentSection = SectionTable::getInstance().getCurrentSection();
  if (currentSection == nullptr) {
    throw AssemblyException("Error: No active section!");
  }
}

void AssemblyDirectives::wordSymbol(string symbol) {
  Section*& currentSection = SectionTable::getInstance().getCurrentSection();
  SymbolTable& symtab = SymbolTable::getInstance();

  if (symtab.foundEntryByName(symbol) == false) {
    throw AssemblyException("Error: Cannot use .word directive with symbol that is not defined!");
  }

  int symbolIndex = symtab.getIndexOfEntry(symbol);
  SymbolTableEntry*& stEntry = symtab[symbolIndex];

  RelocationTable*& relatab = currentSection->getRelaLink();
  int locationCounter = currentSection->getLocationCounter();
  RelocationTableEntry* relaEntry = nullptr;

  if (ST_BIND(stEntry->getInfo()) == STBIND_LOCAL) {
    Utils::addWord(currentSection, stEntry->getValue(), false);
    relaEntry = new RelocationTableEntry(
      locationCounter, RELA_INFO(stEntry->getIndex(), R_X86_64_32), "LOC", stEntry->getValue()
    );
  }
  else if ((ST_BIND(stEntry->getInfo()) == STBIND_GLOBAL) || (stEntry->isDefined() == false)) {
    Utils::addWord(currentSection, 0, false);  
    relaEntry = new RelocationTableEntry(locationCounter, RELA_INFO(symbolIndex, R_X86_64_32), "GLOB");
  }

  if (relaEntry != nullptr && relatab->notAssigned(relaEntry)) {
    relatab->addRelocation(relaEntry);
  }
}

void AssemblyDirectives::wordLiteral(string value) {
  int data = stoi(value, nullptr, Utils::findBase(value));
  Utils::addWord(SectionTable::getInstance().getCurrentSection(), data, false);
}

void AssemblyDirectives::global(AssemblyLineArguments* args) {
  ArgVector arguments = *(args->args);
  
  SymbolTable& symtab = SymbolTable::getInstance();
  for (auto arg : arguments) {
    string value = *(arg.value);

    if (symtab.foundEntryByName(value)) {
      int symbolIndex = symtab.getIndexOfEntry(value);

      if (ST_BIND(symtab[symbolIndex]->getInfo()) == STBIND_GLOBAL) {
        throw AssemblyException("Multiple definition of symbol " + symtab[symbolIndex]->getName());
      }
      if (symtab[symbolIndex]->isExtern()) {
        throw AssemblyException("Error: Symbol cannot have extern and global binding at the same time!");
      }

      symtab[symbolIndex]->setInfo(ST_INFO(STBIND_GLOBAL, STTYPE_NOTYPE));
    }
    else {
      symtab.addSymbol(value, 0, ST_INFO(STBIND_GLOBAL, STTYPE_NOTYPE), UND, 0);
    }
  }
}

void AssemblyDirectives::ext(AssemblyLineArguments* args) {
  ArgVector arguments = *(args->args);

  SymbolTable& symtab = SymbolTable::getInstance();
  for (auto arg : arguments) {
    string value = *(arg.value);
    if (symtab.foundEntryByName(value) == false) {
      symtab.addSymbol(value, 0, ST_INFO(STBIND_EXTERN, STTYPE_NOTYPE), UND, 0);
    }

    SymbolTableEntry*& stEntry = symtab[symtab.getIndexOfEntry(value)];  
    if (stEntry->isDefined()) {
      throw AssemblyException("Symbol defined inside this unit cannot be marked as extern!");
    } 
    if (ST_BIND(stEntry->getInfo()) == STBIND_GLOBAL) {
      throw AssemblyException("Symbol exported as global cannot have external binding!");
    }
    if (stEntry->isExtern()) {
      throw AssemblyException("Mutliple definition of symbol " + stEntry->getName());
    }

    stEntry->setExtern(true);
  }
}

void AssemblyDirectives::sectionPassOne(string* sptr) {
  SectionTable& sectiontab = SectionTable::getInstance();
  
  if (sectiontab.foundSectionByName(*sptr)) {
    throw AssemblyException("Error: Multiple definition of section " + *sptr);
  }

  sectiontab.addSection(*sptr);
  sectiontab.setSection(sectiontab[sectiontab.getSectionIndexByName(*sptr)]);
}

void AssemblyDirectives::sectionPassTwo(string* sptr) {
  SectionTable& sectiontab = SectionTable::getInstance();
  int sectionIndex = sectiontab.getSectionIndexByName(*sptr);
  sectiontab.setSection(sectiontab[sectionIndex]);
}

void AssemblyDirectives::wordPassOne(AssemblyLineArguments* args) {
  checkNoSection();

  Section*& currentSection = SectionTable::getInstance().getCurrentSection();

  int count = args->args->size();
  currentSection->incrementLocationCounter(count * WORD_SIZE);
  currentSection->incrementSize(count * WORD_SIZE);
}

void AssemblyDirectives::wordPassTwo(AssemblyLineArguments* args) {
  ArgVector arguments = *(args->args);
  
  for (auto arg : arguments) {
    string symbolValue = *arg.value;
    if (arg.type == ARG_LIT) { 
      wordLiteral(symbolValue);
    }
    else {
      wordSymbol(symbolValue);
    }

    SectionTable::getInstance().getCurrentSection()->incrementLocationCounter(WORD_SIZE);
  }
}

void AssemblyDirectives::skipPassOne(string literal, int base) {
  checkNoSection();

  Section*& currentSection = SectionTable::getInstance().getCurrentSection();

  int value = stoi(literal, nullptr, base);
  currentSection->incrementLocationCounter(value);
  currentSection->incrementSize(value);
}

void AssemblyDirectives::skipPassTwo(string literal, int base) {
  int value = stoi(literal, nullptr, base);
  Section*& currentSection = SectionTable::getInstance().getCurrentSection();

  for (int i = 0; i < value; i++) {
    currentSection->addByteContent((char)0, 0);
  }
  currentSection->incrementLocationCounter(value);
}

void AssemblyDirectives::asciiPassOne(string str) {
  checkNoSection();

  Section* currentSection = SectionTable::getInstance().getCurrentSection();
  currentSection->incrementLocationCounter(str.length() - 2);
  currentSection->incrementSize(str.length() - 2);
}

void AssemblyDirectives::asciiPassTwo(string str) {
  // treba relokacija i bazen literala za string?
}

void AssemblyDirectives::end() {
  SectionTable& sectiontab = SectionTable::getInstance();
  if (Assembler::getInstance().isFirstPass()) {
    for (int i = 0; i < sectiontab.size(); i++) {
      sectiontab[i]->updateLiteralPool();
    }
  }
  else {
    for (int i = 0; i < sectiontab.size(); i++) {
      sectiontab[i]->copyLiteralPool();
    }

    SymbolTable& symtab = SymbolTable::getInstance();
    for (int i = 0; i < symtab.size(); i++) {
      if (symtab[i]->getIndex() == UND && ST_BIND(symtab[i]->getInfo()) == STBIND_GLOBAL) {
        throw AssemblyException("Symbol " + symtab[i]->getName() + " is marked global but not defined inside this translation unit!");
      }
    }

    ofstream output(Assembler::getInstance().getFileName(), ios::binary);
    if (!output) {
      cerr << "Error opening output file!" << endl;
      return;
    }

    Assembler::getInstance().dumpSectionHeaders(output);
      
    for (int i = 0; i < sectiontab.size(); i++) {
      Assembler::getInstance().dumpSectionData(sectiontab[i], output);
    }

    Assembler::getInstance().dumpSymbolTable(output);

    output.close();

    // for (int i = 0; i < sectiontab.size(); i++) {
    //   sectiontab[i]->printSection();
    //   sectiontab[i]->printRelaTable();

      
    // }
    // cout << "End of assembly file" << endl;
  }

  
}