#ifndef THREE_ADDRESS_CODE_H
#define THREE_ADDRESS_CODE_H

#include "ast.h"
#include "forward_declarations.h"
#include "symbols.h"
#include "types.h"
#include <stdio.h>

// Enum for TAC operations
typedef enum {
  TAC_UNKNOWN,
  TAC_ADD,
  TAC_SUB,
  TAC_MUL,
  TAC_DIV,
  TAC_MOD,
  TAC_LESS,
  TAC_GR,
  TAC_EQ,
  TAC_AND,
  TAC_OR,
  TAC_ASSIGN,
  TAC_LABEL,
  TAC_GOTO,
  TAC_IFZ, // If zero
  TAC_PARAM,
  TAC_CALL,
  TAC_RETURN,
  TAC_PRINT,
  TAC_NOT,
  TAC_NEG,
  TAC_EXTERN
} OpCode;

// Structure for a single Three-Address Code instruction
typedef struct TAC {
  OpCode op;
  Simbolo *op1;
  Simbolo *op2;
  Simbolo *result;
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
void insert_tac(OpCode op, Simbolo *op1, Simbolo *op2, Simbolo *result);

// Function to print the list of TAC instructions
void print_tac_list();

// Helper to get string for op
const char *tac_op_to_string(OpCode op);

Simbolo *get_operand(AST *exp);

// Method to insert into the TAC List for Unary Operations
void unary_operation_insert(OpCode opcode, AST *node);

// Method to insert into the TAC List for Binary Operations
void binary_operation_insert(OpCode opcode, AST *node);

#endif // THREE_ADDRESS_CODE_H
