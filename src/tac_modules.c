#include "../headers/tac_modules.h"

int return_found = 0;
int end_counts = 0;
int else_counts = 0;

char *get_if_name_labels(OpCode type) {
  char *message = malloc(50);
  if (message == NULL)
    return NULL;
  if (type == TAC_LABEL_IF) {
    sprintf(message, "L_else%d", else_counts++);
    return message;
  } else if (type == TAC_LABEL_END) {
    sprintf(message, "L_end%d", end_counts++);
    return message;
  }
  return NULL;
}

void tac_var_dec_module(AST *root, Symbol *exp) {
  exp = get_operand(root->children[0]);
  insert_tac(TAC_ASSIGN, exp, NULL, root->info);
}

void tac_method_dec_module(AST *root) {
  if (root->children[1]->type == TR_EXTERN) {
    insert_tac(TAC_EXTERN, root->info, NULL, NULL);
  } else {
    insert_tac(TAC_LABEL, NULL, NULL, root->info);
    return_found = 0;
    // Parameters of the method
    gen_inter_code(root->children[0]);
    // Body of the method
    gen_inter_code(root->children[1]);
    if (root->info->t_var == T_VOID && !return_found) {
      insert_tac(TAC_RETURN, NULL, NULL, NULL);
      return_found = 0;
    }
  }
}

void tac_param_module(AST *root) {
  insert_tac(TAC_PARAM, root->info, NULL, NULL);
}

void tac_block_module(AST *root) {
  // Generate code of the declarations
  gen_inter_code(root->children[0]);
  // Generate code of the statements
  gen_inter_code(root->children[1]);
}

void tac_assign_module(AST *root, Symbol *exp) {
  exp = get_operand(root->children[1]);
  insert_tac(TAC_ASSIGN, exp, NULL, root->info);
}

void tac_invocation_module(AST *root) {
  // 1. Generate code of the list of arguments (if it exists)
  if (root->child_count > 0 && root->children[0])
    gen_inter_code(root->children[0]);
  // If the function returns something (non-void)
  if (root->info->t_var != T_VOID) {
    // Create a temp to save the result
    char *temp = new_temp();
    Symbol *symbol = malloc(sizeof(Symbol));
    symbol->name = temp;
    symbol->offset = root->info->offset;
    insert_tac(TAC_CALL, root->info, NULL, symbol);
  } else {
    insert_tac(TAC_CALL, root->info, NULL, NULL);
  }
}

void tac_if_statement_module(AST *root, Symbol *L_end, Symbol *exp) {
  // Label creation
  Symbol *L_else = malloc(sizeof(Symbol));
  L_end = malloc(sizeof(Symbol));
  L_else->name = get_if_name_labels(TAC_LABEL_IF);
  L_end->name = get_if_name_labels(TAC_LABEL_END);
  // This if difference between complex condition or simple condition
  exp = get_operand(root->children[0]);
  insert_tac(TAC_IFZ, exp, NULL, L_else);

  // Body of the if
  gen_inter_code(root->children[1]);
  gen_inter_code(root->children[2]);
  insert_tac(TAC_GOTO, NULL, NULL, L_end);

  // else (optional)
  insert_tac(TAC_LABEL_IF, NULL, NULL, L_else);
  gen_inter_code(root->children[3]);

  // end
  insert_tac(TAC_LABEL_END, NULL, NULL, L_end);
}

void tac_else_body_module(AST *root) {
  // First declarations, if they exists
  if (root->children[0] != NULL)
    gen_inter_code(root->children[0]);
  // Then the body of sentences
  if (root->children[1] != NULL)
    gen_inter_code(root->children[1]);
}

void tac_while_statement_module(AST *root, Symbol *L_end, Symbol *exp) {
  Symbol *L_start = malloc(sizeof(Symbol)); // Create labels
  L_end = malloc(sizeof(Symbol));
  L_start->name = "L_start";
  L_end->name = "L_end";
  L_start->offset = 0;
  L_end->offset = 0;
  insert_tac(TAC_LABEL, NULL, NULL, L_start); // 1. Label of start
  exp = get_operand(root->children[0]);       // 2. Condition (same to if)
  insert_tac(TAC_IFZ, exp, NULL, L_end);
  gen_inter_code(root->children[1]);          // 3. Generate body of the while
  insert_tac(TAC_GOTO, NULL, NULL, L_start);  // 4. GOTO the start
  insert_tac(TAC_LABEL, NULL, NULL, L_end);   // 5. Label of end
}

void tac_return_module(AST *root) {
  return_found = 1;
  if (root->child_count == 1) {
    AST *expr = root->children[0];

    // If the expression is simple (identifier or literal)
    if (expr->type == TR_IDENTIFIER || expr->type == TR_VALUE) {
      insert_tac(TAC_RETURN, NULL, NULL, expr->info);
    } else {
      // If its a complex expression: generate his TAC
      gen_inter_code(expr);
      insert_tac(TAC_RETURN, NULL, NULL, tac_list->tail->result);
    }
  } else {
    // Return without expresion (void)
    insert_tac(TAC_RETURN, NULL, NULL, NULL);
  }
}

void tac_args_list_module(AST *root) {
  for (int i = 0; i < root->child_count; i++) {
    Symbol *param = get_operand(root->children[i]);
    insert_tac(TAC_ARG, param, NULL, NULL);
  }
}
