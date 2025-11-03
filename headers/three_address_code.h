#ifndef THREE_ADDRESS_CODE_H
#define THREE_ADDRESS_CODE_H

#include "ast.h"
#include "forward_declarations.h"
#include "symbols.h"
#include "tac_modules.h"
#include "types.h"
#include <stdio.h>

// Structure for a single Three-Address Code instruction
typedef struct TAC {
  OpCode op;
  Symbol *op1;
  Symbol *op2;
  Symbol *result;
  struct TAC *next; // for a linked list implementation
} TAC;

// Structure for the list of TAC instructions
typedef struct TACList {
  TAC *head;
  TAC *tail;
  int count;
} TACList;

// Entry point for generate inter code
void gen_inter_code(AST *root);

// Global TAC list
extern TACList *tac_list;

// Function to initialize the TAC list
void init_tac_list();

// Function to insert a new TAC instruction into the list
void insert_tac(OpCode op, Symbol *op1, Symbol *op2, Symbol *result);

/**
 * Aux method: ensures that returns the Symbol aliasing a Node
 */
Symbol *get_operand(AST *exp);

/**
 * Creates and write a file containing the generated AST
 * It will print it if the debug flag is set
 */
void save_tac_in_file();

/**
 * TAC utility function
 * Prints to the console the generated & saved TAC if the debug flag is set
 */
void print_generated_tac_if_debug_flag();

/**
 * Write the TAC into the global var filename
 */
void write_tac_list_in_file();

// Method to insert into the TAC List for Unary Operations
void unary_operation_insert(OpCode opcode, AST *node);

// Method to insert into the TAC List for Binary Operations
void binary_operation_insert(OpCode opcode, AST *node);

// Helper to get string for op
const char *opcode_to_string(OpCode op);

#endif // THREE_ADDRESS_CODE_H
