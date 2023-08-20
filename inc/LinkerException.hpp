#ifndef LINKER_EXCEPTION_HPP_
#define LINKER_EXCEPTION_HPP_

#include <iostream>
#include <exception>

using namespace std;

class LinkerException : public exception {
private:
  string errorMessage;
public:
  LinkerException(const string& error) : errorMessage(error) { }
  const char* what() const noexcept override {
    return errorMessage.c_str();
  }
};

#endif // LINKER_EXCEPTION_HPP_