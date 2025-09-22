#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "symbols.h"
#include "ast.h"
#include "../syntax.tab.h"

extern FILE *yyin;

int yylex(void);
int compiler_main(int argc, char *argv[]);
void parse_method();
void usage_message(const char *prog);
