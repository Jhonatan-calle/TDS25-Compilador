%{
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include "syntax.tab.h"

  void yyerror(char* msg);
%}

%option noyywrap
%option yylineno

letter                [a-zA-Z]
number                [0-9]+
literals              [-]?{number}
ID 			              {letter}({letter}|{number}|_)*
endOfStatement        ;
operatorDelimiter     [+*<(){}=]
opEquals              "=="
opAnd                 "&&"
opOr                  "||"

/* reserved keywords */
extern                extern
boolType              bool
intType               integer
voidType              void
returnStatement       return
whileLoop             while
beginProgram          program
ifStatement           if
thenStatement         then
elseStatement         else
trueValue             true
falseValue            false

/* regex of ignores */
commentUniline        "//"[^\n]*
commentMultiline      "/*"(.|\n)*"*/"
spacesNewlinesTabs    [ \t\r\n]+
ignoredRegex          {commentUniline}|{commentMultiline}|{spacesNewlinesTabs}


%%

{endOfStatement}        { return T_EOS; }

{literals}              { return V_NUM; }
{trueValue}             { return V_TRUE; }
{falseValue}            { return V_FALSE; }

{extern}                { return R_EXTERN; }
{boolType}              { return R_BOOL; }
{intType}               { return R_INTEGER; }
{voidType}              { return R_VOID; }
{returnStatement}       { return R_RETURN; }
{whileLoop}             { return R_WHILE; }
{beginProgram}          { return R_PROGRAM; }
{ifStatement}           { return R_IF; }
{thenStatement}         { return R_THEN; }
{elseStatement}         { return R_ELSE; }

{opEquals}              { return OP_EQ; }
{opAnd}                 { return OP_AND; }
{opOr}                  { return OP_OR; }

{ID}                    { return ID; }

{operatorDelimiter}     { return *yytext; }

{ignoredRegex}          { /* comments, spaces, newlines and tabs */ }

.                       { yyerror(yytext); }

%%

void yyerror(char* msg) {
  printf("Error: %s -- ", msg);
  printf("At line: %d\n", yylineno);
}
