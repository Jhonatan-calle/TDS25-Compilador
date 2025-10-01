#ifndef THREE_ADDRESS_CODE_H
#define THREE_ADDRESS_CODE_H

#include <stdio.h>
#include "types.h"
#include "ast.h"

// Enum for TAC operations
typedef enum {
  TAC_UNKNOWN,
  TAC_ADD,
  TAC_SUB,
  TAC_MUL,
  TAC_DIV,
  TAC_ASSIGN,
  TAC_LABEL,
  TAC_GOTO,
  TAC_IFZ, // If zero
  TAC_PARAM,
  TAC_CALL,
  TAC_RETURN,
  TAC_PRINT
} OpCode;

// Structure for a single Three-Address Code instruction
typedef struct TAC {
  OpCode op;
  char *result;
  char *op1;
  char *op2;
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
void insert_tac(OpCode op, char *result, char *op1, char *op2);

// Function to print the list of TAC instructions
void print_tac_list();

// Helper to get string for op
const char *tac_op_to_string(OpCode op);

#endif // THREE_ADDRESS_CODE_H
