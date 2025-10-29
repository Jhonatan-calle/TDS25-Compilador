#ifndef ASSEMBLY_CODE_H
#define ASSEMBLY_CODE_H

#include "ast.h"
#include "symbols.h"
#include "three_address_code.h"

/**
 * Assembly util function
 *
 * Called when a new use of a register is used
 * Uses temp_counter global variable to increment or decrement the amount of
 * registers used
 */
char *new_temp();

/**
 * Assembly generate function
 */
void gen_assembly_code(TAC *head);

/**
 * Print the generated assembly if the debug flag is set
 */
void print_generated_assembly_if_debug_flag();
#endif
