#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE *yyin;

int yylex(void);
int compiler_main(int argc, char *argv[]);
void usage(const char *prog);
