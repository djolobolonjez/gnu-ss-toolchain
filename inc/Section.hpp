#ifndef SECTION_HPP_
#define SECTION_HPP_

#include "types.h"
#include <iomanip>

class RelocationTable;

class Section {
private:
  string name;
  
  int id;
  int locationCounter;
  int size;
  
  RelocationTable* relaLink; // add the implementation of rela table entry
  
  ulong vaddr;
  ulong offset;
  
  vector<char> content;
public:
  Section(string _name, int _id);

  string getName() const { return name; }
  int getId() const { return id; }
  int getLocationCounter() const { return locationCounter; }
  int getSize() const { return size; }

  void incrementLocationCounter(int value) { locationCounter += value; }
  void incrementSize(int value) { size += value; }

  void resetLocationCounter() { locationCounter = 0; }

  void addByteContent(char byte) { content.push_back(byte); }

  RelocationTable* getRelaLink() const { return relaLink; }

  void printSection() {
    cout << "Section: " << name << endl;
    for (int address = 0; address < content.size(); address += 8) {
        std::cout << setw(4) << setfill('0') << hex << address << ": ";

        for (int offset = 0; offset < 8; offset++) {
            int byteAddress = address + offset;
            if (byteAddress < content.size())
                std::cout << setw(2) << setfill('0') << hex << ((int)(unsigned char)content[byteAddress] & 0xff);
            else
                std::cout << "  ";

            std::cout << " ";
        }

        std::cout << std::endl;
    }
    cout << endl;
  }
};

#endif // SECTION_HPP_