#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ast.h"
#include "symbols.h"

extern int debug_flag;
extern int assembly_flag;

char *new_temp();
char *gen_code(AST *node);
void print_if_debug_flag(char* str);
void gen_assembly_if_assembly_flag(AST* root);
int process_arguments(int argc, char *argv[], char **outfile, char **target, char **opt, char **inputfile);
