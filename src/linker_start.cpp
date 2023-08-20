#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <iomanip>
#include "../inc/Linker.hpp"

using namespace std;

int main(int argc, char** argv) {
  bool hex = false;
  string filename = "output.hex";
  vector<string> inputFiles;
  map<string, int> sectionsPlace;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-hex") == 0) {
      hex = true;
    } 
    else if (strcmp(argv[i], "-o") == 0) {
      if (i + 1 == argc) {
        cerr << "Missing output hex file name!" << endl;
        return -1;
      }
      filename = argv[i + 1];
      int len = filename.length();
      int pos = filename.find(".");
      if (filename.substr(pos) != ".hex") {
        cerr << "Invalid output file extension!" << endl;
        return -1;
      }
    }
    else {
      string argument = argv[i];
      if (argument.substr(0, 6) == "-place") {
        int len = argument.length();
        string sectionPlace = argument.substr(7);
        int separator = sectionPlace.find("@");
        string section = sectionPlace.substr(0, separator);
        int location = stoi(sectionPlace.substr(separator + 1), nullptr, 16);
        sectionsPlace[section] = location;
      }
      else {
        int dot = argument.find(".");
        if (argument.substr(dot, 2) == ".o") {
          inputFiles.push_back(argument);
        }
      }
    }
  }
  if (!hex) {
    cerr << "Missing -hex option!" << endl;
  }

  Linker& linker = Linker::getInstance();

  linker.setInputFiles(inputFiles);
  linker.setSectionsPlace(sectionsPlace);
  try {
    linker.parseInputFiles();
  }
  catch (const exception& e) {
    cout << e.what() << endl;
  }
  
  return 0;
}