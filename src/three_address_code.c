#include "../headers/three_address_code.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Global TAC list
TACList *tac_list = NULL;

void gen_inter_code(AST *root) {
  if (!root)
    return;

  int type = root->type;

  switch (type) {
  case TR_PROGRAM: {

    // codigo para la lista de declaraciones;
    gen_inter_code(root->childs[0]);
    break;
  }
  case TR_VAR_DECLARATION: {

    Simbolo *exp = get_operand(root->childs[0]);
    insert_tac(TAC_ASSIGN, exp, NULL, root->info);
    break;
  }
  case TR_METHOD_DECLARATION: {

    insert_tac(TAC_LABEL, NULL, NULL, root->info);
    // parametros de metodo
    gen_inter_code(root->childs[0]);
    // si el metodo el local y no externo
    if (root->childs[1]->type == TR_BLOCK) {
      gen_inter_code(root->childs[1]);

      // si no tiene un return igual le pongo uno para marcar
      //  el final del label
      if (root->info->tVar == T_VOID) {
        insert_tac(TAC_RETURN, NULL, NULL, NULL);
      }
    } else {
      insert_tac(TAC_EXTERN, NULL, NULL, NULL);
    }

    break;
  }
  case TR_PARAM: {

    insert_tac(TAC_PARAM, root->info, NULL, NULL);

    break;
  }
  case TR_BLOCK: {

    // generar codigo de declaraciones
    gen_inter_code(root->childs[0]);
    // generar codigo de staments
    gen_inter_code(root->childs[1]);

    break;
  }
  case TR_ASSIGN: {

    Simbolo *exp = get_operand(root->childs[0]);
    insert_tac(TAC_ASSIGN, exp, NULL, root->info);

    break;
  }
  case TR_INVOCATION: {
    // 1. Generar código de la lista de argumentos (si existe)
    if (root->child_count > 0 && root->childs[0])
      gen_inter_code(root->childs[0]);

    // Si la función devuelve algo (no es void)
    if (root->info->tVar != T_VOID) {
      // Crear un temporal para guardar el resultado
      char *temp = new_temp();
      Simbolo *simbol = malloc(sizeof(Simbolo *));
      simbol->nombre = temp;
      insert_tac(TAC_CALL, root->info, root->info, simbol);
    } else {
      insert_tac(TAC_CALL, root->info, root->info, NULL);
    }
    break;
  }
  case TR_IF_STATEMENT: {

    // crecion de labels
    Simbolo *L_else = malloc(sizeof(Simbolo *));
    Simbolo *L_end = malloc(sizeof(Simbolo *));
    L_else->nombre = "L_else";
    L_end->nombre = "L_end";

    // este if diferencia entre condicion compuesta o simple
    Simbolo *exp = get_operand(root->childs[0]);
    insert_tac(TAC_IFZ, exp, NULL, L_else);
    // cuerpo del if
    gen_inter_code(root->childs[1]);
    insert_tac(TAC_GOTO, NULL, NULL, L_end);

    // else (opcional)
    insert_tac(TAC_LABEL, NULL, NULL, L_else);
    gen_inter_code(root->childs[2]);

    // fin
    insert_tac(TAC_LABEL, NULL, NULL, L_end);
    break;
  }
  case TR_ELSE_BODY: {
    // primero declaraciones, si existen
    if (root->childs[0] != NULL) {
      gen_inter_code(root->childs[0]);
    }

    // luego el cuerpo de sentencias
    if (root->childs[1] != NULL) {
      gen_inter_code(root->childs[1]);
    }
    break;
  }
  case TR_WHILE_STATEMENT: {

    Simbolo *L_start = malloc(sizeof(Simbolo)); // Crear labels
    Simbolo *L_end = malloc(sizeof(Simbolo));
    L_start->nombre = "L_start";
    L_end->nombre = "L_end";
    insert_tac(TAC_LABEL, NULL, NULL, L_start); // 1. Label de inicio
    Simbolo *exp =
        get_operand(root->childs[0]); // 2. Condición (igual que en if)
    insert_tac(TAC_IFZ, exp, NULL, L_end);
    gen_inter_code(root->childs[1]); // 3. Generar cuerpo del while
    insert_tac(TAC_GOTO, NULL, NULL, L_start);
    insert_tac(TAC_LABEL, NULL, NULL, L_end); // 5. Label de salida

    break;
  }

  case TR_RETURN: {
    if (root->child_count == 1) {
      AST *expr = root->childs[0];

      // Si la expresión es simple (identificador o literal)
      if (expr->type == TR_IDENTIFIER || expr->type == TR_VALUE) {
        insert_tac(TAC_RETURN, expr->info, NULL, NULL);
      } else {
        // Si es una expresión compuesta: generar su TAC
        gen_inter_code(expr);
        insert_tac(TAC_RETURN, tac_list->tail->result, NULL, NULL);
      }
    } else {
      // return sin expresión (void)
      insert_tac(TAC_RETURN, NULL, NULL, NULL);
    }
    break;
  }
  case TR_LOGIC_NEGATION: {

    // se optiene la expresion a negar
    Simbolo *exp = get_operand(root->childs[0]);
    Simbolo *temp = malloc(sizeof(Simbolo *));
    temp->nombre = new_temp();
    insert_tac(TAC_NOT, exp, NULL, temp);
    break;
  }
  case TR_ARITHMETIC_NEGATION: {
    Simbolo *exp = get_operand(root->childs[0]);
    Simbolo *temp = malloc(sizeof(Simbolo *));
    temp->nombre = new_temp();
    insert_tac(TAC_NEG, exp, NULL, temp);
    break;
  }
  case TR_ADDITION: {
    AST *exp1 = root->childs[0];
    AST *exp2 = root->childs[1];

    Simbolo *op1 = get_operand(exp1);
    Simbolo *op2 = get_operand(exp2);

    Simbolo *temp = malloc(sizeof(Simbolo));
    temp->nombre = new_temp();

    insert_tac(TAC_ADD, op1, op2, temp);
    break;
  }
  case TR_SUBSTRACTION: {
    AST *exp1 = root->childs[0];
    AST *exp2 = root->childs[1];

    Simbolo *op1 = get_operand(exp1);
    Simbolo *op2 = get_operand(exp2);

    Simbolo *temp = malloc(sizeof(Simbolo));
    temp->nombre = new_temp();

    insert_tac(TAC_SUB, op1, op2, temp);
    break;
  }
  case TR_MULTIPLICATION: {
    AST *exp1 = root->childs[0];
    AST *exp2 = root->childs[1];

    Simbolo *op1 = get_operand(exp1);
    Simbolo *op2 = get_operand(exp2);

    Simbolo *temp = malloc(sizeof(Simbolo));
    temp->nombre = new_temp();

    insert_tac(TAC_MUL, op1, op2, temp);
    break;
  }
  case TR_DIVITION: {
    AST *exp1 = root->childs[0];
    AST *exp2 = root->childs[1];

    Simbolo *op1 = get_operand(exp1);
    Simbolo *op2 = get_operand(exp2);

    Simbolo *temp = malloc(sizeof(Simbolo));
    temp->nombre = new_temp();

    insert_tac(TAC_DIV, op1, op2, temp);
    break;
  }
  case TR_MODULO: {
    AST *exp1 = root->childs[0];
    AST *exp2 = root->childs[1];

    Simbolo *op1 = get_operand(exp1);
    Simbolo *op2 = get_operand(exp2);

    Simbolo *temp = malloc(sizeof(Simbolo));
    temp->nombre = new_temp();

    insert_tac(TAC_MOD, op1, op2, temp);
    break;
  }
  case TR_LESS_THAN: {
    AST *exp1 = root->childs[0];
    AST *exp2 = root->childs[1];

    Simbolo *op1 = get_operand(exp1);
    Simbolo *op2 = get_operand(exp2);

    Simbolo *temp = malloc(sizeof(Simbolo));
    temp->nombre = new_temp();

    insert_tac(TAC_LESS, op1, op2, temp);
    break;
  }
  case TR_GREATER_THAN: {
    AST *exp1 = root->childs[0];
    AST *exp2 = root->childs[1];

    Simbolo *op1 = get_operand(exp1);
    Simbolo *op2 = get_operand(exp2);

    Simbolo *temp = malloc(sizeof(Simbolo));
    temp->nombre = new_temp();

    insert_tac(TAC_GR, op1, op2, temp);
    break;
  }
  case TR_LOGIC_EQUAL: {
    AST *exp1 = root->childs[0];
    AST *exp2 = root->childs[1];

    Simbolo *op1 = get_operand(exp1);
    Simbolo *op2 = get_operand(exp2);

    Simbolo *temp = malloc(sizeof(Simbolo));
    temp->nombre = new_temp();

    insert_tac(TAC_EQ, op1, op2, temp);
    break;
  }
  case TR_AND: {

    AST *exp1 = root->childs[0];
    AST *exp2 = root->childs[1];

    Simbolo *op1 = get_operand(exp1);
    Simbolo *op2 = get_operand(exp2);

    Simbolo *temp = malloc(sizeof(Simbolo));
    temp->nombre = new_temp();

    insert_tac(TAC_AND, op1, op2, temp);
    break;
  }
  case TR_OR: {
    AST *exp1 = root->childs[0];
    AST *exp2 = root->childs[1];

    Simbolo *op1 = get_operand(exp1);
    Simbolo *op2 = get_operand(exp2);

    Simbolo *temp = malloc(sizeof(Simbolo));
    temp->nombre = new_temp();

    insert_tac(TAC_OR, op1, op2, temp);
    break;
  }
  case TR_ARG_LIST:
    for (int i = 0; i < root->child_count; i++) {
      Simbolo *param = get_operand(root->childs[i]);
      insert_tac(TAC_PARAM, param, NULL, NULL);
    }
    break;
  case TR_DECLARATION_LIST:
  case TR_PARAM_LIST:
  case TR_SENTENCES_LIST:
    for (int i = 0; i < root->child_count; i++)
      gen_inter_code(root->childs[i]);
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

// auxiliar: asegura que devuelve el símbolo asociado a un nodo
Simbolo *get_operand(AST *exp) {
  if (exp->type == TR_IDENTIFIER || exp->type == TR_VALUE) {
    return exp->info;
  } else {
    gen_inter_code(exp);
    return tac_list->tail->result;
  }
}

// Function to print the list of TAC instructions
void print_tac_list() {
  if (!tac_list || !tac_list->head) {
    printf("\n===== INTERMEDIATE CODE (TAC) =====\n");
    printf("No TAC instructions generated.\n");
    printf("===================================\n");
    return;
  }

  TAC *current = tac_list->head;

  printf("\n===== INTERMEDIATE CODE (TAC) =====\n");

  while (current) {
    const char *op_name = tac_op_to_string(current->op); // función auxiliar
    const char *arg1 = current->op1 ? current->op1->nombre : "_";
    const char *arg2 = current->op2 ? current->op2->nombre : "_";
    const char *res  = current->result  ? current->result->nombre  : "_";

    printf("%-10s %-10s %-10s %-10s\n", op_name, arg1, arg2, res);

    current = current->next;
  }

  printf("===================================\n");
}


// Helper to get string for op
const char *tac_op_to_string(OpCode op) {
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
  case TAC_IFZ:
    return "TAC_IFZ";
  case TAC_PARAM:
    return "TAC_PARAM";
  case TAC_CALL:
    return "TAC_CALL";
  case TAC_RETURN:
    return "TAC_RETURN";
  case TAC_PRINT:
    return "TAC_PRINT";
  case TAC_NOT:
    return "TAC_NOT";
  case TAC_NEG:
    return "TAC_NEG";
  case TAC_EXTERN:
    return "TAC_EXTERN";
  default:
    return "UNKNOWN";
  }
}
