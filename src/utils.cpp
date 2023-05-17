#include "../inc/utils.hpp"

namespace Utils {
  AssemblyLineArguments* create_arguments() {
    return new AssemblyLineArguments();
  }

  int findBase(string value) {
    int base = 10;
    if (value.substr(0,2) == "0x") {
      base = 16;
    }
    else if (value.substr(0,1) == "0") {
      base = 8;
    }

    return base;
  }
}