#include "../headers/tac_modules.h"

void tac_var_dec_module(AST *root, Symbol *exp) {
  exp = get_operand(root->childs[0]);
  insert_tac(TAC_ASSIGN, exp, NULL, root->info);
}

void tac_method_dec_module(AST *root) {
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
}

void tac_param_module(AST *root) {
  insert_tac(TAC_PARAM, root->info, NULL, NULL);
}

void tac_block_module(AST *root) {
  // generar codigo de declaraciones
  gen_inter_code(root->childs[0]);
  // generar codigo de staments
  gen_inter_code(root->childs[1]);
}

void tac_assign_module(AST *root, Symbol *exp) {
  exp = get_operand(root->childs[0]);
  insert_tac(TAC_ASSIGN, exp, NULL, root->info);
}

void tac_invocation_module(AST *root) {
  // 1. Generar código de la lista de argumentos (si existe)
  if (root->child_count > 0 && root->childs[0])
    gen_inter_code(root->childs[0]);
  // Si la función devuelve algo (no es void)
  if (root->info->tVar != T_VOID) {
    // Crear un temporal para guardar el resultado
    char *temp = new_temp();
    Symbol *simbol = malloc(sizeof(Symbol));
    simbol->nombre = temp;
    insert_tac(TAC_CALL, root->info, root->info, simbol);
  } else {
    insert_tac(TAC_CALL, root->info, root->info, NULL);
  }
}

void tac_if_statement_module(AST *root, Symbol *L_end, Symbol *exp) {
  // crecion de labels
  Symbol *L_else = malloc(sizeof(Symbol));
  L_end = malloc(sizeof(Symbol));
  L_else->nombre = "L_else";
  L_end->nombre = "L_end";
  // este if diferencia entre condicion compuesta o simple
  exp = get_operand(root->childs[0]);
  insert_tac(TAC_IFZ, exp, NULL, L_else);
  // cuerpo del if
  gen_inter_code(root->childs[1]);
  insert_tac(TAC_GOTO, NULL, NULL, L_end);
  // else (opcional)
  insert_tac(TAC_LABEL, NULL, NULL, L_else);
  gen_inter_code(root->childs[2]);
  // fin
  insert_tac(TAC_LABEL, NULL, NULL, L_end);
}

void tac_else_body_module(AST *root) {
  // primero declaraciones, si existen
  if (root->childs[0] != NULL)
    gen_inter_code(root->childs[0]);
  // luego el cuerpo de sentencias
  if (root->childs[1] != NULL)
    gen_inter_code(root->childs[1]);
}

void tac_while_statement_module(AST *root, Symbol *L_end, Symbol *exp) {
  Symbol *L_start = malloc(sizeof(Symbol)); // Crear labels
  L_end = malloc(sizeof(Symbol));
  L_start->nombre = "L_start";
  L_end->nombre = "L_end";
  insert_tac(TAC_LABEL, NULL, NULL, L_start); // 1. Label de inicio
  exp = get_operand(root->childs[0]);         // 2. Condición (igual que en if)
  insert_tac(TAC_IFZ, exp, NULL, L_end);
  gen_inter_code(root->childs[1]); // 3. Generar cuerpo del while
  insert_tac(TAC_GOTO, NULL, NULL, L_start);
  insert_tac(TAC_LABEL, NULL, NULL, L_end); // 5. Label de salida
}

void tac_return_module(AST *root) {
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
}

void tac_args_list_module(AST *root) {
  for (int i = 0; i < root->child_count; i++) {
    Symbol *param = get_operand(root->childs[i]);
    insert_tac(TAC_PARAM, param, NULL, NULL);
  }
}
