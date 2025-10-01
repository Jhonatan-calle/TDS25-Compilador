#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../syntax.tab.h"
#include "ast.h"
#include "symbols.h"

extern int debug_flag;
extern int assembly_flag;
extern int inter_code_flag;

char *new_temp();
char *gen_assembly_code(AST *node);
void gen_inter_code(AST* root);
void print_if_debug_flag(char *str);
void gen_assembly_if_assembly_flag(AST *root);
void gen_inter_code_if_inter_code_flag(AST* root);
int process_arguments(int argc, char *argv[], char **outfile, char **target,
                      char **opt, char **inputfile);
void parse_method();
void usage_message(const char *prog);
int process_target_stage(const char *target);
int create_output_file(const char *outfile);
