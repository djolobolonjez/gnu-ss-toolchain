%code requires{
#include <cstdio>
#include "inc/types.h"
#include "inc/utils.h"
#include "inc/directives_wrapper.h"

extern int yylex();
extern int yyparse();
extern FILE* yyin;

extern int line_num;

void yyerror(const char* s);

}

%{ 
%}

%output "parser.cpp"
%defines "parser.hpp"

%union {
  int ival;
  string* sval;
  AssemblyLineArguments* args;
}

%token TOKEN_ENDL TOKEN_COMMA
%token TOKEN_GLOBAL TOKEN_END

%token <sval> TOKEN_SYM

%type <args> symlist

%%

input: 
  | line input
  ;

line:
  endls
  | TOKEN_GLOBAL symlist endls { _asm_dir_global($2); delete $2; }
  | TOKEN_END { _asm_dir_end(); return 15; }
  ;

symlist:
  TOKEN_SYM { $$ = create_arguments(); $$->args->push_back($1); }
  | symlist TOKEN_COMMA TOKEN_SYM { $1->args->push_back($3); $$ = $1; }
  ;

endls: 
  endls TOKEN_ENDL
  | TOKEN_ENDL ;

%%

int main(int argc, char** argv) {
  FILE* fp = fopen("tests/test.s", "r");
  
  if (!fp) {
    cout << "File not found!" << endl;
    return -1;
  }

  yyin = fp;

  while (yyparse() != 15) { }

  return 0;
}

void yyerror(const char* s) {
  cout << "Parse error on line " << line_num << endl << "Message: " << s << endl;
  exit(-1);
}