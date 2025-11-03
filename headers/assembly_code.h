#ifndef ASSEMBLY_CODE_H
#define ASSEMBLY_CODE_H

#include "ast.h"
#include "symbols.h"
#include "three_address_code.h"
#include "assembly_modules.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Resets the global local and temp slots to zero
 */
void enter_function();

/**
 * Allocate a new local storage entry for the current function/frame.
 * Return the new amount of local slots
 */
int alloc_local();

/**
 * Allocs a temp *after* the locals. Returns a unique negative offset
 *    Stores the temp after the locals:
 *    offset = -((local_slots + temp_index) * 8)
 */
int alloc_temp();

/**
 * Compute and assign memory offsets for AST declarations
 *
 * Go through the AST and compute the offsets for declarations,
 * parameters and other entities that need storage.
 */
void gen_offsets(AST *root);

/**
 * Assembly util function
 *
 * Called when a new use of a register is used
 * Uses temp_counter global variable to increment or decrement the amount of
 * registers used
 */
char *new_temp();

/**
 * Returns the next argument register available
 */
const char *get_arg_register();

/**
 * Resets the use of argument registers (after a call)
 */
void reset_arg_registers();

/**
 * Assembly util function
 * It generates the assembly code using the generated TAC List
 */
void gen_assembly_code(TAC *head);

/**
 * Assembly utility function
 * Prints to the console the generated & saved assembly if the debug flag is set
 */
void print_generated_assembly_if_debug_flag();

#endif
