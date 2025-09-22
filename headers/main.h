#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE *yyin;
extern int debug;

int yylex(void);
int compiler_main(int argc, char *argv[]);
void parse_method();
void usage_message(const char *prog);
