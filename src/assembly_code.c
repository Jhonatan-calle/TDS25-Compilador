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

  switch (tac_instruction->op) {
  case TAC_ADD:
    update_offset();
    tac_instruction->result->offset = DYNAMIC_OFFSET;
    printf("ADD !! result = %s\n", symbol_to_string(tac_instruction->result));
    break;
  case TAC_SUB:
    update_offset();
    tac_instruction->result->offset = DYNAMIC_OFFSET;
    printf("SUB !! result = %s\n", symbol_to_string(tac_instruction->result));
    break;
  case TAC_MUL:
    update_offset();
    tac_instruction->result->offset = DYNAMIC_OFFSET;
    printf("MUL !! result = %s\n", symbol_to_string(tac_instruction->result));
    break;
  case TAC_DIV:
    update_offset();
    tac_instruction->result->offset = DYNAMIC_OFFSET;
    printf("DIV !! result = %s\n", symbol_to_string(tac_instruction->result));
    break;
  case TAC_MOD:
    update_offset();
    tac_instruction->result->offset = DYNAMIC_OFFSET;
    printf("MOD !! result = %s\n", symbol_to_string(tac_instruction->result));
    break;
  case TAC_LESS:
    update_offset();
    tac_instruction->result->offset = DYNAMIC_OFFSET;
    printf("LESS !! result = %s\n", symbol_to_string(tac_instruction->result));
    break;
  case TAC_GR:
    update_offset();
    tac_instruction->result->offset = DYNAMIC_OFFSET;
    printf("GR !! result = %s\n", symbol_to_string(tac_instruction->result));
    break;
  case TAC_EQ:
    update_offset();
    tac_instruction->result->offset = DYNAMIC_OFFSET;
    printf("EQ !! result = %s\n", symbol_to_string(tac_instruction->result));
    break;
  case TAC_AND:
    update_offset();
    tac_instruction->result->offset = DYNAMIC_OFFSET;
    printf("AND !! result = %s\n", symbol_to_string(tac_instruction->result));
    break;
  case TAC_OR:
    update_offset();
    tac_instruction->result->offset = DYNAMIC_OFFSET;
    printf("OR !! result = %s\n", symbol_to_string(tac_instruction->result));
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
    update_offset();
    tac_instruction->result->offset = DYNAMIC_OFFSET;
    printf("NOT !! result = %s\n", symbol_to_string(tac_instruction->result));
    break;
  case TAC_NEG:
    update_offset();
    tac_instruction->result->offset = DYNAMIC_OFFSET;
    printf("NEG !! result = %s\n", symbol_to_string(tac_instruction->result));
    break;
  case TAC_UNKNOWN:
  case TAC_EXTERN:
  default:
    return;
  }
  tac_instruction = tac_instruction->next;
  gen_assembly_code(tac_instruction);
}