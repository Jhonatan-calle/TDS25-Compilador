#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "symbols.h"
#include "ast.h"

extern FILE *yyin;

int yylex(void);
int compiler_main(int argc, char *argv[]);
