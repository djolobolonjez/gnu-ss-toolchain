#ifndef EMULATOR_HPP_
#define EMULATOR_HPP_

#include "types.h"
#include <map>
#include <iostream>

using namespace std;

typedef map<long, int> Memory;

class Emulator {

private:
  Emulator();
  Registers registers;

  Memory memory;
public:
  static Emulator& getInstance();
  static unsigned int toLittleEndian(unsigned int);
  static unsigned int fromLittleEndian(unsigned int);
  Registers& getRegisters() { return registers; }
  Memory& getMemory() { return memory; }
  void printProcessorState();
};


#endif // EMULATOR_HPP_