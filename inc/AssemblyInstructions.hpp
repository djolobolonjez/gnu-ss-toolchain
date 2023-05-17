#ifndef ASSEMBLY_INSTRUCTIONS_HPP_
#define ASSEMBLY_INSTUCTIONS_HPP_

class AssemblyInstructions {

private:
  AssemblyInstructions() = default;
public:
  static AssemblyInstructions& getInstance();
};

#endif // ASSEMBLY_INSTRUCTIONS_HPP_