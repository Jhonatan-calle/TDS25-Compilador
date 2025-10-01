#include "../headers/three_address_code.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Global TAC list
TACList *tac_list = NULL;

void gen_inter_code(AST *root) {
  init_tac_list();

  int type = root->type;

  switch (type) {
  case TR_PROGRAM:
      gen_inter_code(root->childs[0]);
    break;
  case TR_VAR_DECLARATION:
      insert_tac(TAC_ASSIGN,root->info,NULL,root->info);
    break;
  case TR_METHOD_DECLARATION:
    break;
  case TR_PARAM:
    break;
  case TR_PARAM_LIST:
    break;
  case TR_BLOCK:
    break;
  case TR_ASSIGN:
    break;
  case TR_INVOCATION:
    break;
  case TR_IF_STATEMENT:
    break;
  case TR_ELSE_BODY:
    break;
  case TR_WHILE_STATEMENT:
    break;
  case TR_RETURN:
    break;
  case TR_LOGIC_NEGATION:
    break;
  case TR_ARITHMETIC_NEGATION:
    break;
  case TR_ADDITION:
    break;
  case TR_SUBSTRACTION:
    break;
  case TR_MULTIPLICATION:
    break;
  case TR_DIVITION:
    break;
  case TR_MODULO:
    break;
  case TR_LESS_THAN:
    break;
  case TR_GREATER_THAN:
    break;
  case TR_LOGIC_EQUAL:
    break;
  case TR_AND:
    break;
  case TR_OR:
    break;
  case TR_IDENTIFIER:
    break;
  case TR_VALUE:
    break;
  case TR_ARG_LIST:
    break;
  case TR_DECLARATION_LIST:
      for(int i = 0; i < root->child_count;)
        gen_inter_code(root->childs[0]);
    break;
  default:
    fprintf(stderr, "Warning: Tipo de nodo no manejado en new_node: %s\n",
            tipoNodoToStr(type));
    break;
  }

  if (debug_flag) {
    printf("[DEBUG NEW_NODE] Nodo %s finalizado, child_count=%d\n",
           tipoNodoToStr(type), root->child_count);
  }

  return;
}

// Function to initialize the TAC list
void init_tac_list() {
  tac_list = (TACList *)malloc(sizeof(TACList));
  if (tac_list == NULL) {
    fprintf(stderr, "Failed to allocate memory for TAC list\n");
    exit(EXIT_FAILURE);
  }
  tac_list->head = NULL;
  tac_list->tail = NULL;
  tac_list->count = 0;
}

// Function to insert a new TAC instruction into the list
void insert_tac(OpCode op, Simbolo *op1, Simbolo *op2, Simbolo *result) {
  if (tac_list == NULL) {
    init_tac_list();
  }

  TAC *new_tac = (TAC *)malloc(sizeof(TAC));
  if (new_tac == NULL) {
    fprintf(stderr, "Failed to allocate memory for new TAC instruction\n");
    exit(EXIT_FAILURE);
  }

  new_tac->op = op;
  new_tac->result = result;
  new_tac->op1 = op1;
  new_tac->op2 = op2;
  new_tac->next = NULL;

  if (tac_list->head == NULL) {
    tac_list->head = new_tac;
    tac_list->tail = new_tac;
  } else {
    tac_list->tail->next = new_tac;
    tac_list->tail = new_tac;
  }
  tac_list->count++;
}

// Function to print the list of TAC instructions
void print_tac_list() {
  //TODO
}

// // Helper to get string for op
// const char *tac_op_to_string(OpCode op) {
//   switch (op) {
//   case TAC_ADD:
//     return "+";
//   case TAC_SUB:
//     return "-";
//   case TAC_MUL:
//     return "*";
//   case TAC_DIV:
//     return "/";
//   case TAC_ASSIGN:
//     return "ASSIGN";
//   case TAC_LABEL:
//     return "LABEL";
//   case TAC_GOTO:
//     return "GOTO";
//   case TAC_IFZ:
//     return "IFZ";
//   case TAC_PARAM:
//     return "PARAM";
//   case TAC_CALL:
//     return "CALL";
//   case TAC_RETURN:
//     return "RETURN";
//   case TAC_PRINT:
//     return "PRINT";
//   default:
//     return "UNKNOWN";
//   }
// }
