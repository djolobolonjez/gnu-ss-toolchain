#ifndef ASSEMBLY_INSTRUCTIONS_HPP_
#define ASSEMBLY_INSTUCTIONS_HPP_

#include <iostream>
using namespace std;

struct InstructionArguments;

class AssemblyInstructions {

private:
  AssemblyInstructions() = default;
  
  struct opcode {
    int first, second, third, displacement;
    opcode(int f = 0, int s = 0, int t = 0, int disp = 0) : first(f), second(s), third(t), displacement(disp) { }
  };

public:
  static AssemblyInstructions& getInstance();
  void firstPass();

  void halt();
  void interrupt();
  void xchg(string, string);
  void arithmetic(int, string, string);
  void logical(int, string, string);
  void shl(string, string);
  void shr(string, string);
  void ld(InstructionArguments*, opcode);
  void push(InstructionArguments*);
  void pop(InstructionArguments*);
};

#endif // ASSEMBLY_INSTRUCTIONS_HPP_