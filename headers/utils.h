#include <stdlib.h>
#include <string.h>
#include "ast.h"

char *new_temp();
char *gen_code(AST *node);
void print_if_debug(char* str);
