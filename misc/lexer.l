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
[ \t\r]+
"#"[^\n\r]* 
, { return TOKEN_COMMA; }
":" { return TOKEN_COLON; }
\.global { return TOKEN_GLOBAL; }
\.extern { return TOKEN_EXTERN; }
\.section { return TOKEN_SECTION; }
\.end { return TOKEN_END; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.sval = new string(yytext); return TOKEN_SYM; }
\n { ++line_num; return TOKEN_ENDL; }
. { }

%%