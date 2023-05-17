#include "../inc/AssemblyInstructions.hpp"

AssemblyInstructions& AssemblyInstructions::getInstance() {
  static AssemblyInstructions instance;
  return instance;
}