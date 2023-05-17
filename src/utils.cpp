#include "../inc/utils.hpp"
#include "../inc/defs.h"
#include "../inc/Section.hpp"
#include "../inc/SectionTable.hpp"

namespace Utils {
  AssemblyLineArguments* create_arguments() {
    return new AssemblyLineArguments();
  }

  int findBase(string value) {
    int base = 10;
    if (value.substr(0,2) == "0x") {
      base = 16;
    }
    else if (value.substr(0,1) == "0") {
      base = 8;
    }

    return base;
  }

  void addWord(Section*& section, int data) {
    for (int i = 0; i < WORD_SIZE; i++) {
      section->addByteContent(BYTEMASK(data));
      data >>= 8;
    }
  
  }

  void toBytesHex(stringstream& ss) {
    int data;
    ss >> hex >> data;
    Utils::addWord(SectionTable::getInstance().getCurrentSection(), data);
  }
}