#include "../headers/three_address_code.h"

// Global TAC list
TACList *tac_list = NULL;

void gen_inter_code(AST *root) {
  if (!root)
    return;

  int type = root->type;
  Symbol *exp = NULL;
  Symbol *L_end = NULL;

  switch (type) {
  case TR_PROGRAM:
    // codigo para la lista de declaraciones;
    gen_inter_code(root->childs[0]);
    break;

  case TR_VAR_DECLARATION:
    tac_var_dec_module(root, exp);
    break;

  case TR_METHOD_DECLARATION:
    tac_method_dec_module(root);
    break;

  case TR_PARAM:
    tac_param_module(root);
    break;

  case TR_BLOCK:
    tac_block_module(root);
    break;

  case TR_ASSIGN:
    tac_assign_module(root, exp);
    break;

  case TR_INVOCATION:
    tac_invocation_module(root);
    break;

  case TR_IF_STATEMENT:
    tac_if_statement_module(root, L_end, exp);
    break;

  case TR_ELSE_BODY:
    tac_else_body_module(root);
    break;

  case TR_WHILE_STATEMENT:
    tac_while_statement_module(root, L_end, exp);
    break;

  case TR_RETURN:
    tac_return_module(root);
    break;

  case TR_LOGIC_NEGATION:
    unary_operation_insert(TAC_NOT, root);
    break;

  case TR_ARITHMETIC_NEGATION:
    unary_operation_insert(TAC_NEG, root);
    break;

  case TR_ADDITION:
    binary_operation_insert(TAC_ADD, root);
    break;

  case TR_SUBSTRACTION:
    binary_operation_insert(TAC_SUB, root);
    break;

  case TR_MULTIPLICATION:
    binary_operation_insert(TAC_MUL, root);
    break;

  case TR_DIVITION:
    binary_operation_insert(TAC_DIV, root);
    break;

  case TR_MODULO:
    binary_operation_insert(TAC_MOD, root);
    break;

  case TR_LESS_THAN:
    binary_operation_insert(TAC_LESS, root);
    break;

  case TR_GREATER_THAN:
    binary_operation_insert(TAC_GR, root);
    break;

  case TR_LOGIC_EQUAL:
    binary_operation_insert(TAC_EQ, root);
    break;

  case TR_AND:
    binary_operation_insert(TAC_AND, root);
    break;

  case TR_OR:
    binary_operation_insert(TAC_OR, root);
    break;

  case TR_ARG_LIST:
    tac_args_list_module(root);
    break;

  case TR_DECLARATION_LIST:
  case TR_PARAM_LIST:
  case TR_SENTENCES_LIST:
    for (int i = 0; i < root->child_count; i++)
      gen_inter_code(root->childs[i]);
    break;

  default:
    break;
  }

  if (debug_flag) {
    printf("[DEBUG Gen_Inter_Code] Nodo %s finalizado, child_count=%d\n",
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
void insert_tac(OpCode op, Symbol *op1, Symbol *op2, Symbol *result) {
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

// auxiliar: asegura que devuelve el símbolo asociado a un nodo
Symbol *get_operand(AST *exp) {
  if (exp->type == TR_IDENTIFIER || exp->type == TR_VALUE) {
    return exp->info;
  } else {
    gen_inter_code(exp);
    return tac_list->tail->result;
  }
}

// Function to print the list of TAC instructions
void print_tac_list() {
  printf("\n===== INTERMEDIATE CODE (TAC) =====\n");

  if (!tac_list || !tac_list->head) {
    printf("No TAC instructions generated.\n");
  } else {
    TAC *current = tac_list->head;
    while (current) {
      const char *op_name = opcode_to_string(current->op); // función auxiliar
      const char *arg1 = current->op1 ? current->op1->nombre : "_";
      const char *arg2 = current->op2 ? current->op2->nombre : "_";
      const char *res = current->result ? current->result->nombre : "_";

      printf("%-10s %-10s %-10s %-10s\n", op_name, arg1, arg2, res);

      current = current->next;
    }
  }

  printf("\n===================================\n");
}

// Method to insert into the TAC List for Unary Operations
void unary_operation_insert(OpCode opcode, AST *node) {
  Symbol *exp = get_operand(node->childs[0]);
  Symbol *temp = malloc(sizeof(Symbol));
  temp->nombre = new_temp();
  temp->offset = node->info->offset;

  insert_tac(opcode, exp, NULL, temp);
}

// Method to insert into the TAC List for Binary Operations
void binary_operation_insert(OpCode opcode, AST *node) {
  AST *exp1 = node->childs[0];
  AST *exp2 = node->childs[1];

  Symbol *op1 = get_operand(exp1);
  Symbol *op2 = get_operand(exp2);

  Symbol *temp = malloc(sizeof(Symbol));
  temp->nombre = new_temp();
  temp->offset = node->info->offset;

  insert_tac(opcode, op1, op2, temp);
}

// Helper to get string for op
const char *opcode_to_string(OpCode op) {
  switch (op) {
  case TAC_UNKNOWN:
    return "TAC_UNKNOWN";
  case TAC_ADD:
    return "TAC_ADD";
  case TAC_SUB:
    return "TAC_SUB";
  case TAC_MUL:
    return "TAC_MUL";
  case TAC_DIV:
    return "TAC_DIV";
  case TAC_MOD:
    return "TAC_MOD";
  case TAC_LESS:
    return "TAC_LESS";
  case TAC_GR:
    return "TAC_GR";
  case TAC_EQ:
    return "TAC_EQ";
  case TAC_AND:
    return "TAC_AND";
  case TAC_OR:
    return "TAC_OR";
  case TAC_ASSIGN:
    return "TAC_ASSIGN";
  case TAC_LABEL:
    return "TAC_LABEL";
  case TAC_GOTO:
    return "TAC_GOTO";
  case TAC_LABEL_IF:
    return "TAC_LABEL_IF";
  case TAC_LABEL_END:
    return "TAC_LABEL_END";
  case TAC_IFZ:
    return "TAC_IFZ";
  case TAC_PARAM:
    return "TAC_PARAM";
  case TAC_ARG:
    return "TAC_ARG";
  case TAC_CALL:
    return "TAC_CALL";
  case TAC_RETURN:
    return "TAC_RETURN";
  case TAC_NOT:
    return "TAC_NOT";
  case TAC_NEG:
    return "TAC_NEG";
  case TAC_EXTERN:
    return "TAC_EXTERN";
  default:
    return "";
  }
}
