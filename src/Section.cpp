#include "../inc/Section.hpp"
#include "../inc/SectionTable.hpp"
#include "../inc/RelocationTable.hpp"
#include "../inc/defs.h"

Section::Section(string _name, int _id) : name(_name), id(_id) {
  size = 0;
  locationCounter = 0;
  relaLink = new RelocationTable();
  offset = 0;
  vaddr = 0;
}

void Section::printRelaTable() {
    cout << "------------------------------------" << endl;
    cout << endl << "#.rela." << name << endl;

    cout << "Offset" << "\t" << "Type" << "\t" << "Symbol" << "\t" << "Addend";
    cout << endl;

    for (int i = 0; i < relaLink->size(); i++) {
      cout << (*relaLink)[i]->offset << "\t";
      if (RELA_TYPE((*relaLink)[i]->info) == R_X86_64_32) {
        cout << "R_X86_64_32";
      }
      cout << "\t";
      cout << RELA_SYM((*relaLink)[i]->info) << "\t";
      cout << (*relaLink)[i]->addend << endl;
    }
  }