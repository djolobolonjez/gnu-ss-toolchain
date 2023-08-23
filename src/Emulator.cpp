#include "../inc/Emulator.hpp"
#include "../inc/defs.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

Emulator::Emulator() {
  for (int i = 0; i < 16; i++) {
    this->registers.gpr[i] = 0;
  }
  for (int i = 0; i < 3; i++) {
    this->registers.csr[i] = 0;
  }
}

Emulator& Emulator::getInstance() {
  static Emulator instance;
  return instance;
}

void Emulator::printProcessorState() {
  cout << "----------------------------------------" << endl;
  cout << "Emulated processor executed halt instruction" << endl;
}

int main(int argc, char** argv) {
  Emulator& emulator = Emulator::getInstance();

  if (argc != 2) {
    cerr << "You must provide memory content!" << endl;
    return -1;
  }

  const char* inputFile = argv[1];

  ifstream input(inputFile);

  if (!input.is_open()) {
    cerr << "Unable to read memory file!" << endl;
    return -1;
  }

  Memory& memory = emulator.getMemory();
  Registers& registers = emulator.getRegisters();

  string line;
  while (getline(input, line)) {
    string delimiter = ": ";
    size_t pos = line.find_first_of(delimiter);

    long address = stol(line.substr(0, pos), nullptr, 16);
    string content = line.substr(pos + 2);
    for (int i = 0; i < 4; i++) {
      stringstream ss;
      ss << hex << content.substr(2 * i, 2);
      unsigned int data;
      ss >> data;
      char memoryContent = static_cast<char>(data);
      memory[address] = memoryContent;
      // cout << setw(8) << hex << setfill('0') << address + i << ": ";
      // std::cout << 
      //       std::hex << std::setw(2) << std::setfill('0')
      //       << static_cast<int>(static_cast<unsigned char>(memoryContent))
      //       << std::endl;
    }
  }
  input.close();

  registers.gpr[pc] = 0x40000000;
  bool running = true;
  int nextInstruction;

  while (running) {
    nextInstruction = memory[registers.gpr[pc]++];
    cout << hex << setw(8) << setfill('0') << registers.gpr[pc] << endl;

    switch (nextInstruction) {
      case HALT: {
        running = false;
        break;
      }
    }
  }

  emulator.printProcessorState();

  return 0;
}