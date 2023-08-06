#ifndef SECTION_HPP_
#define SECTION_HPP_

#include "types.h"
#include <iomanip>
#include <map>

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
  vector<char> literalPoolContent;
  map<string, int> literalPool;
public:
  Section(string _name, int _id);

  map<string, int> getPool() { return literalPool; }
  int getPoolSize() const { return literalPoolContent.size(); }

  string getName() const { return name; }
  int getId() const { return id; }
  int getLocationCounter() const { return locationCounter; }
  int getSize() const { return size; }

  void incrementLocationCounter(int value) { locationCounter += value; }
  void incrementSize(int value) { size += value; }

  void resetLocationCounter() { locationCounter = 0; }

  void addByteContent(char byte, int container) { 
    if (container == 0) {
      content.push_back(byte); 
    }
    else {
      literalPoolContent.push_back(byte);
    }
  }

  void copyLiteralPool() {
    for (int i = 0; i < literalPoolContent.size(); i++) {
      content.push_back(literalPoolContent[i]);
    }
  }

  RelocationTable*& getRelaLink() { return relaLink; }

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
    for (int address = 0; address < literalPoolContent.size(); address += 8) {
        std::cout << setw(4) << setfill('0') << hex << address << ": ";

        for (int offset = 0; offset < 8; offset++) {
            int byteAddress = address + offset;
            if (byteAddress < literalPoolContent.size())
                std::cout << setw(2) << setfill('0') << hex << ((int)(unsigned char)literalPoolContent[byteAddress] & 0xff);
            else
                std::cout << "  ";

            std::cout << " ";
        }

        std::cout << std::endl;
    }
    cout << endl;
  }

  void printRelaTable();
};

#endif // SECTION_HPP_