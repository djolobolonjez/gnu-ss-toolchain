#include "../inc/AssemblyDirectives.hpp"
#include "../inc/SymbolTable.hpp"
#include "../inc/defs.h"
#include "../inc/SectionTable.hpp"
#include "../inc/Section.hpp"
#include "../inc/AssemblyException.hpp"

AssemblyDirectives& AssemblyDirectives::getInstance() {
  static AssemblyDirectives instance;
  return instance;
}

void AssemblyDirectives::checkNoSection() {
  Section* currentSection = SectionTable::getInstance().getCurrentSection();
  if (currentSection == nullptr) {
    throw AssemblyException("Error: No active section!");
  }
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

void AssemblyDirectives::section(string* sptr) {
  SectionTable& sectiontab = SectionTable::getInstance();
  
  if (sectiontab.foundSectionByName(*sptr)) {
    throw AssemblyException("Error: Multiple definition of section " + *sptr);
  }

  sectiontab.addSection(*sptr);
  sectiontab.setSection(sectiontab[sectiontab.getSectionIndexByName(*sptr)]);
}

void AssemblyDirectives::word(AssemblyLineArguments* args) {
  checkNoSection();

  Section* currentSection = SectionTable::getInstance().getCurrentSection();
  // Treba sacuvati simbole i literale za koje se poziva ova direktiva, zbog drugog prolaza asembler
  int count = args->args->size();
  currentSection->incrementLocationCounter(count * 4);
  currentSection->incrementSize(count * 4);
}

void AssemblyDirectives::skip(string literal, int base) {
  checkNoSection();

  Section* currentSection = SectionTable::getInstance().getCurrentSection();

  int value = stoi(literal, nullptr, base);
  currentSection->incrementLocationCounter(value);
  currentSection->incrementSize(value);
}

void AssemblyDirectives::ascii(string str) {
  checkNoSection();

  Section* currentSection = SectionTable::getInstance().getCurrentSection();
  currentSection->incrementLocationCounter(str.length() - 2);
  currentSection->incrementSize(str.length() - 2);
}

void AssemblyDirectives::end() {
  cout << "End of assembly file" << endl;
}