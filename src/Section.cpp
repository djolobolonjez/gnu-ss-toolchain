#include "../inc/Section.hpp"
#include "../inc/SectionTable.hpp"

Section::Section(string _name, int _id) : name(_name), id(_id) {
  size = 0;
  locationCounter = 0;
  relaLink = nullptr;
  offset = 0;
  vaddr = 0;
}