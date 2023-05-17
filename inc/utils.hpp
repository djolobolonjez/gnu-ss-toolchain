#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <iostream>
#include "../inc/types.h"
using namespace std;

struct AssemblyLineArguments;

namespace Utils {

  AssemblyLineArguments* create_arguments();
  int findBase(string value);
}

#endif // UTILS_H_