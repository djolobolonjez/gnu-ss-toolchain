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

%token TOKEN_ENDL TOKEN_COMMA TOKEN_COLON
%token TOKEN_GLOBAL TOKEN_EXTERN TOKEN_SECTION TOKEN_WORD TOKEN_SKIP TOKEN_ASCII TOKEN_END 

%token <sval> TOKEN_SYM
%token <sval> TOKEN_LITERAL
%token <sval> TOKEN_STRING

%type <args> symlist

%%

input: 
  | line input
  ;

line:
  endls
  | TOKEN_SYM TOKEN_COLON TOKEN_GLOBAL symlist endls { _asm_label($1); _asm_dir_global($4); delete $1; delete $4; }
  | TOKEN_GLOBAL symlist endls { _asm_dir_global($2); delete $2; }

  | TOKEN_SYM TOKEN_COLON TOKEN_EXTERN symlist endls { _asm_label($1); _asm_dir_extern($4); delete $4; delete $1; }
  | TOKEN_EXTERN symlist endls { _asm_dir_extern($2); delete $2; } 

  | TOKEN_SYM TOKEN_COLON TOKEN_SECTION TOKEN_SYM endls { _asm_label($1); _asm_dir_section($4); delete $1; delete $4; }
  | TOKEN_SECTION TOKEN_SYM endls { _asm_dir_section($2); delete $2; }

  | TOKEN_SYM TOKEN_COLON endls { _asm_label($1); delete $1; }

  | TOKEN_SYM TOKEN_COLON TOKEN_WORD symlist endls { _asm_label($1); _asm_dir_word($4); delete $1; delete $4; } 
  | TOKEN_WORD symlist endls { _asm_dir_word($2); delete $2; }

  | TOKEN_SYM TOKEN_COLON TOKEN_SKIP TOKEN_LITERAL endls { _asm_label($1); _asm_dir_skip($4); delete $1; delete $4; }
  | TOKEN_SKIP TOKEN_LITERAL endls { _asm_dir_skip($2); delete $2; }

  | TOKEN_SYM TOKEN_COLON TOKEN_ASCII TOKEN_STRING endls { _asm_label($1); _asm_dir_ascii($4); delete $1; delete $4; }
  | TOKEN_ASCII TOKEN_STRING endls { _asm_dir_ascii($2); delete $2; }

  | TOKEN_SYM TOKEN_COLON TOKEN_END { _asm_label($1); delete $1; _asm_dir_end(); return 15; }
  | TOKEN_END { _asm_dir_end(); return 15; }
  ;

symlist:
  TOKEN_SYM { $$ = create_arguments(); $$->args->push_back({$1, 0}); }
  | TOKEN_LITERAL { $$ = create_arguments(); $$->args->push_back({$1, 1}); }
  | symlist TOKEN_COMMA TOKEN_LITERAL { $1->args->push_back({$3, 1}); $$ = $1; }
  | symlist TOKEN_COMMA TOKEN_SYM { $1->args->push_back({$3, 0}); $$ = $1; }
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

  try {
    while(yyparse() != 15) { }
  }
  catch (const exception& e) {
    cout << e.what() << endl;
  }

  return 0;
}

void yyerror(const char* s) {
  cout << "Parse error on line " << line_num << endl << "Message: " << s << endl;
  exit(-1);
}