#ifndef EMULATOR_HPP_
#define EMULATOR_HPP_

#include "types.h"
#include <map>

typedef map<long, char> Memory;

class Emulator {

private:
  Emulator();
  Registers registers;

  Memory memory;
public:
  static Emulator& getInstance();
  Registers& getRegisters() { return registers; }
  Memory& getMemory() { return memory; }
  void printProcessorState();
};


#endif // EMULATOR_HPP_