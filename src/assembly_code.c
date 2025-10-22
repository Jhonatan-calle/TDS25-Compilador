#include "../headers/assembly_code.h"

int temp_counter = 0;

/**
 * Assembly util function
 *
 * Called when a new use of a register is used
 * Uses temp_counter global variable to increment or decrement the amount of
 * registers used
 */
char *new_temp() {
  char buffer[16];
  snprintf(buffer, sizeof(buffer), "t%d", temp_counter++);
  return strdup(buffer);
}

/**
 * Assembly util function
 *
 * Called on the top level of the program
 * It constructs and prints a pseudo-assembly recursively
 */
void gen_assembly_code(TAC *tac_instruction) {
  // Case base: No more instructions
  if (!tac_instruction)
    return;

  printf("OpCode = %s \n", opcode_to_string(tac_instruction->op));
  printf("op1 = %s\n", symbol_to_string(tac_instruction->op1));
  printf("op2 = %s\n", symbol_to_string(tac_instruction->op2));
  printf("result = %s\n", symbol_to_string(tac_instruction->result));

  tac_instruction = tac_instruction->next;
  gen_assembly_code(tac_instruction);

  // if (tac_instruction->op == TAC_RETURN )

  switch (tac_instruction->op) {
  case TAC_ADD:
    break;
  case TAC_SUB:
    break;
  case TAC_MUL:
    break;
  case TAC_DIV:
    break;
  case TAC_MOD:
    break;
  case TAC_LESS:
    break;
  case TAC_GR:
    break;
  case TAC_EQ:
    break;
  case TAC_AND:
    break;
  case TAC_OR:
    break;
  case TAC_ASSIGN:
    break;
  case TAC_LABEL:
    break;
  case TAC_GOTO:
    break;
  case TAC_IFZ:
    break;
  case TAC_PARAM:
    break;
  case TAC_CALL:
    break;
  case TAC_RETURN:
    return;
  case TAC_PRINT:
    break;
  case TAC_NOT:
    break;
  case TAC_NEG:
    break;
  case TAC_UNKNOWN:
  case TAC_EXTERN:
  default:
    return;
  }
}