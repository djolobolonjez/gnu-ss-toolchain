#ifndef ASSEMBLY_INSTRUCTIONS_HPP_
#define ASSEMBLY_INSTUCTIONS_HPP_

#include <iostream>
using namespace std;

class AssemblyInstructions {

private:
  AssemblyInstructions() = default;
public:
  static AssemblyInstructions& getInstance();
  void firstPass();

  void halt();
  void interrupt();
  void xchg(string, string);
  void arithmetic(int, string, string);
  void logical(int, string, string);
};

#endif // ASSEMBLY_INSTRUCTIONS_HPP_