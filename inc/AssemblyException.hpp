#ifndef ASSEMBLY_EXCEPTION_HPP_
#define ASSEMBLY_EXCEPTION_HPP_

#include <iostream>
#include <exception>

using namespace std;

class AssemblyException : public exception {
private:
  string errorMessage;
public:
  AssemblyException(const string& error) : errorMessage(error) { }
  const char* what() const noexcept override {
    return errorMessage.c_str();
  }
};

#endif // ASSEMBLY_EXCEPTION_HPP_