#include "../inc/Assembler.hpp"
#include "../inc/AssemblyDirectives.hpp"
#include "../inc/AssemblyInstructions.hpp"
#include "../inc/SymbolTable.hpp"
#include "../inc/SectionTable.hpp"
#include "../inc/Section.hpp"
#include "../inc/defs.h"
#include "../inc/AssemblyException.hpp"
#include "../inc/RelocationTable.hpp"
#include "../inc/types.h"
#include <iomanip>
#include <fstream>

Assembler& Assembler::getInstance() {
  static Assembler instance;
  return instance;
}

AssemblyDirectives& Assembler::directives() {
  return AssemblyDirectives::getInstance();
}

AssemblyInstructions& Assembler::instructions() {
  return AssemblyInstructions::getInstance();
}

int Assembler::label(string* labelPtr) {
  SectionTable& sectiontab = SectionTable::getInstance();
  if (sectiontab.getCurrentSection() == nullptr) {
    throw AssemblyException("Symbol has to be defined inside an existing section!");
  }

  ulong locationCounter = sectiontab.getCurrentSection()->getLocationCounter();
  int sectionIndex = sectiontab.getCurrentSection()->getId();

  SymbolTable& symtab = SymbolTable::getInstance();

  if (symtab.foundEntryByName(*labelPtr)) {
    SymbolTableEntry*& stEntry = symtab[symtab.getIndexOfEntry(*labelPtr)];

    if (stEntry->isDefined() || stEntry->isExtern()) {
      throw AssemblyException("Symbol " + stEntry->getName() + " is already defined!" );
    }

    // otherwise, symbol was mentioned before definition, so we define it now

    stEntry->setIndex(sectionIndex);
    stEntry->setDefined(true);
    stEntry->setValue(locationCounter);
  }
  else {
    symtab.addSymbol(*labelPtr, locationCounter, ST_INFO(STBIND_LOCAL, STTYPE_NOTYPE), sectionIndex, 0);
    symtab[symtab.getIndexOfEntry(*labelPtr)]->setDefined(true);
  }

  return 0;
}

void Assembler::sectionReset() {
  SectionTable& sectiontab = SectionTable::getInstance();

  for (int i = 0; i < sectiontab.size(); i++) {
    sectiontab[i]->resetLocationCounter();
  }
  secondPass();
}

void Assembler::dumpSectionData(Section*& section, ofstream& buff) {
  buff << "---\n" << section->getName() << endl;
  vector<char>& data = section->getContent();
  int counter = 0;
  for (auto const byte : data) {
    buff << setw(2) << setfill('0') << hex << static_cast<int>(static_cast<unsigned char>(byte)) << " ";
    if (++counter == 8) {
      counter %= 8;
      buff << '\n';
    }
  }
  RelocationTable& relatable = *(section->getRelaLink());
  buff << ".rela." << section->getName() << endl;
  for (int i = 0; i < relatable.size(); i++) {
    buff << "--";
    buff << relatable[i]->offset << " " << RELA_TYPE(relatable[i]->info) << " ";
    buff << RELA_SYM(relatable[i]->info) << " " << relatable[i]->addend << "\n";
  }
}

void Assembler::dumpSymbolTable(ofstream& buff) {
  SymbolTable& symtab = SymbolTable::getInstance();
  buff << "\n#.symtab" << "\n";
  for (int i = 0; i < symtab.size(); i++) {
    SymbolTableEntry*& entry = symtab[i];
    buff << "--";
    buff << hex << i << " " << entry->value << " " << entry->size << " " << ST_TYPE(entry->info) << " " << 
    ST_BIND(entry->info) << " " << entry->index << " ";
    buff << entry->name << "\n"; 
  }
}

void Assembler::printSymbolTable() {
  SymbolTable& symtab = SymbolTable::getInstance();

  cout << "------------------------------------" << endl;
  cout << endl << "#.symtab" << endl;

  cout << "Num" << "\t" << "Value" << "\t" << "Size" << "\t" << "Type" << "\t" 
  << "Bind" << "\t" << "Index" << "\t" << "Name";
  cout << endl;

  for (int i = 0; i < symtab.size(); i++) {
    SymbolTableEntry*& stEntry = symtab[i];
    cout << i << "\t";
    cout << hex << stEntry->value << "\t";
    cout << stEntry->size << "\t";

    cout << (ST_TYPE(stEntry->info) == STTYPE_NOTYPE ? "NOTYPE" : "SECTION");
    cout << "\t";

    if (ST_BIND(stEntry->info) == STBIND_GLOBAL) {
      cout << "GLOB";
    }
    else if (ST_BIND(stEntry->info) == STBIND_EXTERN) {
      cout << "EXT";
    }
    else { 
      cout << "LOC";
    }
    cout << "\t";

    if (stEntry->index == UND) {
      cout << "UND";
    }
    else {
      cout << stEntry->index;
    }
    cout << "\t" << stEntry->name << endl;
  }
  cout << "------------------------------------" << endl;
}