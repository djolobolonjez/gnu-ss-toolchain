#include "../inc/AssemblyDirectives.hpp"
#include "../inc/SymbolTable.hpp"
#include "../inc/defs.h"
#include "../inc/SectionTable.hpp"
#include "../inc/Section.hpp"

AssemblyDirectives& AssemblyDirectives::getInstance() {
  static AssemblyDirectives instance;
  return instance;
}

void AssemblyDirectives::global(AssemblyLineArguments* args) {
  ArgVector arguments = *(args->args);
  
  SymbolTable& symtab = SymbolTable::getInstance();
  for (auto sptr : arguments) {
    if (symtab.foundEntryByName(*sptr)) {
      int symbolIndex = symtab.getIndexOfEntry(*sptr);
      symtab[symbolIndex]->setInfo(ST_INFO(STBIND_GLOBAL, STTYPE_NOTYPE));
    }
    else {
      symtab.addSymbol(*sptr, 0, ST_INFO(STBIND_GLOBAL, STTYPE_NOTYPE), UND, 0);
    }
  }
}

int AssemblyDirectives::ext(AssemblyLineArguments* args) {
  ArgVector arguments = *(args->args);

  SymbolTable& symtab = SymbolTable::getInstance();
  for (auto sptr : arguments) {
    if (symtab.foundEntryByName(*sptr) == false) {
      symtab.addSymbol(*sptr, 0, ST_INFO(STBIND_GLOBAL, STTYPE_NOTYPE), UND, 0);
    }

    SymbolTableEntry*& stEntry = symtab[symtab.getIndexOfEntry(*sptr)];  
    if (stEntry->isDefined()) {
      cout << "Symbol defined inside this unit cannot be marked as extern!" << endl;
      return -1;
    }
    stEntry->setExtern(true);
  }

  return 0;
}

void AssemblyDirectives::section(string* sptr) {
  SectionTable& sectiontab = SectionTable::getInstance();
  
  Section*& currentSection = sectiontab.getCurrentSection();
  if (currentSection == nullptr) {
    sectiontab.addSection(*sptr);
    currentSection = sectiontab[sectiontab.getSectionIndexByName(*sptr)];
  }
  else {
    if (currentSection->getName() != *sptr) {
      if (sectiontab.foundSectionByName(*sptr) == false) {
        sectiontab.addSection(*sptr);
      }
      currentSection = sectiontab[sectiontab.getSectionIndexByName(*sptr)];
    }
  }
}

void AssemblyDirectives::end() {
  cout << "End of assembly file" << endl;
}