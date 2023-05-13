%{
#include "parser.hpp"
#include <iostream>
#include <string>
using namespace std;
struct AssemblyLineArguments;

int line_num = 1;
%}

%option outfile="lexer.cpp" header-file="lexer.hpp"

%option noyywrap 


%%
, { return TOKEN_COMMA; }
^.global { return TOKEN_GLOBAL; }
^end { return TOKEN_END; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.sval = new string(yytext); return TOKEN_SYM; }
\n { ++line_num; return TOKEN_ENDL; }
. {}

%%