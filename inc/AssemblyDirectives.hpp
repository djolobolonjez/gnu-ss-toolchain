#ifndef ASSEMBLY_DIRECTIVES_HPP_
#define ASSEMBLY_DIRECTIVES_HPP_

struct AssemblyLineArguments;

class AssemblyDirectives {
private:
  AssemblyDirectives() = default;
public:
  static AssemblyDirectives& getInstance();

  void global(AssemblyLineArguments*);
  void end();
};

#endif // ASSEMBLY_DIRECTIVES_HPP_