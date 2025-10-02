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

    // si la expresion es un identificador o un litteral
    if (root->childs[0]->type == TR_IDENTIFIER ||
        root->childs[0]->type == TR_VALUE) {

      insert_tac(TAC_ASSIGN, root->childs[0]->info, NULL, root->info);

      // sino que primero resuelva la expresion y despues asigne el
      // el resultado
    } else {

      gen_inter_code(root->childs[0]);
      insert_tac(TAC_ASSIGN, tac_list->tail->result, NULL, root->info);
    }

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

    // si la expresion es un identificador o un litteral
    if (root->childs[1]->type == TR_IDENTIFIER ||
        root->childs[1]->type == TR_VALUE) {

      insert_tac(TAC_ASSIGN, root->childs[0]->info, NULL, root->info);

      // sino que primero resuelva la expresion y despues asigne el
      // el resultado
    } else {

      gen_inter_code(root->childs[0]);
      insert_tac(TAC_ASSIGN, tac_list->tail->result, NULL, root->info);
    }

    break;
  }
  case TR_INVOCATION: {
    // 1. Generar código de la lista de argumentos (si existe)
    if (root->child_count > 0 && root->childs[0]) {
      gen_inter_code(root->childs[0]);
    }

    // Si la función devuelve algo (no es void)
    if (root->info->tVar != T_VOID) {
      // Crear un temporal para guardar el resultado
      char *temp = new_temp();
      Simbolo *simbol = malloc(sizeof(Simbolo *));
      simbol->nombre = temp;
      insert_tac(TAC_CALL, root->info, root->info, simbol);

    } else {
      // Si es void, no necesitamos temp
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
    if (root->childs[0]->type == TR_IDENTIFIER ||
        root->childs[0]->type == TR_VALUE) {

      insert_tac(TAC_IFZ, root->childs[0]->info, NULL, L_else);

    } else {

      gen_inter_code(root->childs[0]);
      insert_tac(TAC_IFZ, tac_list->tail->result, NULL, L_else);
    }

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
    // Crear labels
    Simbolo *L_start = malloc(sizeof(Simbolo));
    Simbolo *L_end = malloc(sizeof(Simbolo));
    L_start->nombre = "L_start";
    L_end->nombre = "L_end";

    // 1. Label de inicio
    insert_tac(TAC_LABEL, NULL, NULL, L_start);

    // 2. Condición (igual que en if)
    if (root->childs[0]->type == TR_IDENTIFIER ||
        root->childs[0]->type == TR_VALUE) {
      insert_tac(TAC_IFZ, root->childs[0]->info, NULL, L_end);
    } else {
      gen_inter_code(root->childs[0]);
      insert_tac(TAC_IFZ, tac_list->tail->result, NULL, L_end);
    }

    // 3. Generar cuerpo del while
    gen_inter_code(root->childs[1]);

    // 4. Volver al inicio
    insert_tac(TAC_GOTO, NULL, NULL, L_start);

    // 5. Label de salida
    insert_tac(TAC_LABEL, NULL, NULL, L_end);

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
  case TR_ARG_LIST:
    break;
  case TR_IDENTIFIER:
    break;
  case TR_VALUE:
    break;
  case TR_DECLARATION_LIST:
  case TR_PARAM_LIST:
  case TR_SENTENCES_LIST:
    for (int i = 0; i < root->child_count;)
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
  // TODO
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
