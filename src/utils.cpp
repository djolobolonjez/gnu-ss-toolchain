#include "../inc/utils.hpp"
#include "../inc/defs.h"
#include "../inc/Section.hpp"
#include "../inc/SectionTable.hpp"

namespace Utils {
  AssemblyLineArguments* create_arguments() {
    return new AssemblyLineArguments();
  }

  InstructionArguments* create_instruction(int mod) {
    return new InstructionArguments(mod);
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

  void addWord(Section*& section, unsigned data, bool endianness, int container) {
    if (endianness) {
      int offset = 24;
      for (int i = 0; i < WORD_SIZE; i++) {
        section->addByteContent((data >> offset) & 0xff, container);
        offset -= (2 * WORD_SIZE);
      }
    }
    else {
      for (int i = 0; i < WORD_SIZE; i++) {
        section->addByteContent(BYTEMASK(data), container);
        data >>= 8;
      }
    }
  }

  void toBytesHex(stringstream& ss, bool endianness) {
    unsigned data;
    ss >> hex >> data;
    Utils::addWord(SectionTable::getInstance().getCurrentSection(), data, endianness);
  }
}