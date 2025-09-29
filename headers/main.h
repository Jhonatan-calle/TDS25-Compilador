#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "symbols.h"
#include "utils.h"

extern FILE *yyin;

int yylex(void);
int compiler_main(int argc, char *argv[]);
