#include "../inc/Emulator.hpp"
#include "../inc/defs.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <bitset>

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
  cout << "Emulated processor state:" << endl;
  for (int i = 0; i < 16; i++) {
    cout << "r" << to_string(i) << ": " << "0x";
    cout << hex << setw(8) << setfill('0') << registers.gpr[i] << "\t";
    if ((i + 1) % 4 == 0) {
      cout << endl;
    }
  }
}

unsigned int Emulator::toLittleEndian(unsigned int num) {
  unsigned char bytes[4];
  bytes[0] = static_cast<unsigned char>(num & 0xff);
  bytes[1] = static_cast<unsigned char>((num >> 8) & 0xff);
  bytes[2] = static_cast<unsigned char>((num >> 16) & 0xff);
  bytes[3] = static_cast<unsigned char>((num >> 24) & 0xff);

  return bytes[3] | (static_cast<unsigned int>(bytes[2]) << 8) |
                    (static_cast<unsigned int>(bytes[1]) << 16) |
                    (static_cast<unsigned int>(bytes[0]) << 24);
}

unsigned int Emulator::fromLittleEndian(unsigned int num) {
  unsigned char bytes[4];
  bytes[0] = static_cast<unsigned char>(num & 0xff);
  bytes[1] = static_cast<unsigned char>((num >> 8) & 0xff);
  bytes[2] = static_cast<unsigned char>((num >> 16) & 0xff);
  bytes[3] = static_cast<unsigned char>((num >> 24) & 0xff);

  return bytes[3] | (static_cast<unsigned int>(bytes[2]) << 8) |
                    (static_cast<unsigned int>(bytes[1]) << 16) |
                    (static_cast<unsigned int>(bytes[0]) << 24);
}


int main(int argc, char** argv) {
  Emulator& emulator = Emulator::getInstance();

  /*if (argc != 2) {
    cerr << "You must provide memory content!" << endl;
    return -1;
  }*/

  const char* inputFile = "program.hex";

  ifstream input(inputFile);

  if (!input.is_open()) {
    cerr << "Unable to read memory file!" << endl;
    return -1;
  }

  Memory& memory = emulator.getMemory();
  Registers& registers = emulator.getRegisters();

  registers.gpr[r1] = 2;
  registers.gpr[r2] = 3; 

  string line;
  while (getline(input, line)) {
    string delimiter = ": ";
    size_t pos = line.find_first_of(delimiter);

    long address = stol(line.substr(0, pos), nullptr, 16);
    string content = line.substr(pos + 2);
    stringstream ss;
    ss << hex << content;
    unsigned int data;
    ss >> hex >> data;
    memory[address] = data;
  }
  input.close();

  registers.gpr[pc] = 0x40000000;
  registers.gpr[sp] = 0xffffff00;
  registers.csr[handler] = 0xf0000000;
  bool running = true;
  int nextInstruction;

  while (running) {
    nextInstruction = memory[registers.gpr[pc]];
    registers.gpr[pc] += 4;

    int opcode = (nextInstruction & 0xff000000) >> 24;
    int byteTwo = (nextInstruction & 0x00ff0000) >> 16;
    int byteThree = (nextInstruction & 0x0000ff00) >> 8;
    int byteFour = nextInstruction & 0xff;  

    int regA = (byteTwo >> 4) & 0xf, regB = byteTwo & 0xf, regC = (byteThree >> 4) & 0xf;
    int displacement = byteFour | (((byteThree & 0xf) << 28) >> 20);

    switch (opcode) {
      case HALT: {
        running = false;
        break;
      }
      case INT: {
        registers.gpr[sp] -= 4;
        memory[registers.gpr[sp]] = Emulator::toLittleEndian(registers.csr[status]);
        registers.gpr[sp] -= 4;
        memory[registers.gpr[sp]] = Emulator::toLittleEndian(registers.gpr[pc]);
        registers.csr[cause] = 0x4;
        registers.csr[status] &= (~0x1);
        registers.gpr[pc] = registers.csr[handler];
        break;
      }
      case XCHG: {
        if (regB == 0 || regC == 0) {
          break;
        }
        int tmp = registers.gpr[regB];
        registers.gpr[regB] = registers.gpr[regC];
        registers.gpr[regC] = tmp;
        break;
      }
      case ADD: {
        if (regA == 0) {
          break;
        }
        registers.gpr[regA] = registers.gpr[regB] + registers.gpr[regC];
        break;
      }
      case SUB: {
        if (regA == 0) {
          break;
        }
        registers.gpr[regA] = registers.gpr[regC] - registers.gpr[regB];
        break;
      }
      case MUL: {
        if (regA == 0) {
          break;
        }
        registers.gpr[regA] = registers.gpr[regB] * registers.gpr[regC];
        break;
      }
      case DIV: {
        if (regA == 0) {
          break;
        }
        registers.gpr[regA] = registers.gpr[regC] / registers.gpr[regB];
        break;
      }
      case NOT: {
        if (regA == 0) {
          break;
        }
        registers.gpr[regA] = ~registers.gpr[regB];
        break;
      }
      case AND: {
        if (regA == 0) {
          break;
        }
        registers.gpr[regA] = registers.gpr[regB] & registers.gpr[regC];
        break;
      }
      case OR: {
        if (regA == 0) {
          break;
        }
        registers.gpr[regA] = registers.gpr[regB] | registers.gpr[regC];
        break;
      }
      case XOR: {
        if (regA == 0) {
          break;
        }
        registers.gpr[regA] = registers.gpr[regB] ^ registers.gpr[regC];
        break;
      }
      case SHL: {
        if (regA == 0) {
          break;
        }
        registers.gpr[regA] = registers.gpr[regB] << registers.gpr[regC];
        break;
      }
      case SHR: {
        if (regA == 0) {
          break;
        }
        registers.gpr[regA] = registers.gpr[regB] >> registers.gpr[regC];
        break;
      }
      case CALL: {
       registers.gpr[sp] -= 4;
       memory[registers.gpr[sp]] = Emulator::toLittleEndian(registers.gpr[pc]);
       long address = registers.gpr[regA] + registers.gpr[regB] + displacement;
       registers.gpr[pc] = Emulator::fromLittleEndian(memory[address]);
       break;
      }
      case JMP: {
        long address = registers.gpr[regA] + displacement;
        registers.gpr[pc] = Emulator::fromLittleEndian(memory[address]);
        break;
      }
      case BEQ: {
        long address = registers.gpr[regA] + displacement;
        if (registers.gpr[regB] == registers.gpr[regC]) {
          registers.gpr[pc] = Emulator::fromLittleEndian(memory[address]);
        }
        break;
      }
      case BNE: {
        long address = registers.gpr[regA] + displacement;
        if (registers.gpr[regB] != registers.gpr[regC]) {
          registers.gpr[pc] = Emulator::fromLittleEndian(memory[address]);
        }
        break;
      }
      case BGT: {
        long address = registers.gpr[regA] + displacement;
        if (registers.gpr[regB] > registers.gpr[regC]) {
          registers.gpr[pc] = Emulator::fromLittleEndian(memory[address]);
        }
        break;
      }
      case LD_CSR: {
        registers.gpr[regA] = registers.csr[regB];
        break;
      }
      case LD_CSRW: {
        registers.csr[regA] = registers.gpr[regB];
        break;
      }
      case LD_REG: {
        registers.gpr[regA] = registers.gpr[regB] + displacement;
        break;
      }
      case LD_MEM: {
        long address = registers.gpr[regB] + registers.gpr[regC] + displacement;
        registers.gpr[regA] = Emulator::fromLittleEndian(memory[address]);

        break;
      }
      case LD_MEM_REG: {
        registers.gpr[regA] = Emulator::fromLittleEndian(memory[registers.gpr[regB]]);
        registers.gpr[regB] += displacement;
        break;
      }
      case ST_MEMREG: {
        registers.gpr[regA] += displacement;
        memory[registers.gpr[regA]] = Emulator::toLittleEndian(registers.gpr[regC]);
      }
      case ST_MEM: {
        long address = registers.gpr[regA] + registers.gpr[regB] + displacement;
        memory[address] = Emulator::toLittleEndian(registers.gpr[regC]);
        break;
      }
      case ST_MEMIND: {
        long address = registers.gpr[regA] + registers.gpr[regB] + displacement;
        memory[Emulator::fromLittleEndian(memory[address])] = Emulator::toLittleEndian(registers.gpr[regC]); 
        break;
      }
    }
  }

  emulator.printProcessorState();

  return 0;
}