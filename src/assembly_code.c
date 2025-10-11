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
char *gen_assembly_code(AST *node) {
  // Case base: Node NULL
  if (!node)
    return NULL;

  switch (node->type) {
  // Case base: Leaf
  case TR_VALUE: {
    char *t = new_temp();
    printf("LOAD %s, %d\n", t, node->info->valor);
    return t;
  }

  // Case base: Leaf
  case TR_IDENTIFIER: {
    char *t = new_temp();
    printf("LOAD %s, %s\n", t, node->info->nombre);
    return t;
  }

  case TR_VAR_DECLARATION: {
    if (node->child_count > 0 && node->childs[0])
      gen_assembly_code(node->childs[0]);
    return NULL;
  }

  // Recursive step on the right side expression
  case TR_ASSIGN: {
    // child[0] = identificador
    // child[1] = expresión
    char *rhs = gen_assembly_code(node->childs[1]);
    printf("STORE %s, %s\n", node->childs[0]->info->nombre, rhs);
    return rhs;
  }

  // Recursive step on both sides
  case TR_ADDITION: {
    char *lhs = gen_assembly_code(node->childs[0]);
    char *rhs = gen_assembly_code(node->childs[1]);
    char *t = new_temp();
    printf("ADD %s, %s, %s\n", t, lhs, rhs);
    return t;
  }

  case TR_SUBSTRACTION: {
    char *lhs = gen_assembly_code(node->childs[0]);
    char *rhs = gen_assembly_code(node->childs[1]);
    char *t = new_temp();
    printf("SUB %s, %s, %s\n", t, lhs, rhs);
    return t;
  }

  // Recursive step on both sides
  case TR_MULTIPLICATION: {
    char *lhs = gen_assembly_code(node->childs[0]);
    char *rhs = gen_assembly_code(node->childs[1]);
    char *t = new_temp();
    printf("MUL %s, %s, %s\n", t, lhs, rhs);
    return t;
  }

  case TR_DIVITION: {
    char *lhs = gen_assembly_code(node->childs[0]);
    char *rhs = gen_assembly_code(node->childs[1]);
    char *t = new_temp();
    printf("DIV %s, %s, %s\n", t, lhs, rhs);
    return t;
  }

  case TR_MODULO: {
    char *lhs = gen_assembly_code(node->childs[0]);
    char *rhs = gen_assembly_code(node->childs[1]);
    char *t = new_temp();
    printf("MOD %s, %s, %s\n", t, lhs, rhs);
    return t;
  }

  case TR_LESS_THAN:
  case TR_GREATER_THAN:
  case TR_LOGIC_EQUAL: {
    char *lhs = gen_assembly_code(node->childs[0]);
    char *rhs = gen_assembly_code(node->childs[1]);
    char *t = new_temp();
    char *op = (node->type == TR_LESS_THAN)      ? "LT"
               : (node->type == TR_GREATER_THAN) ? "GT"
                                                 : "EQ";
    printf("%s %s, %s, %s\n", op, t, lhs, rhs);
    return t;
  }

  // Recursive step on both sides
  case TR_AND: {
    char *lhs = gen_assembly_code(node->childs[0]);
    char *rhs = gen_assembly_code(node->childs[1]);
    char *t = new_temp();
    printf("AND %s, %s, %s\n", t, lhs, rhs);
    return t;
  }

  // Recursive step on both sides
  case TR_OR: {
    char *lhs = gen_assembly_code(node->childs[0]);
    char *rhs = gen_assembly_code(node->childs[1]);
    char *t = new_temp();
    printf("OR %s, %s, %s\n", t, lhs, rhs);
    return t;
  }

  // Parameter
  case TR_PARAM:
    if (node->child_count > 0 && node->childs[0])
      gen_assembly_code(node->childs[0]);
    return NULL;

  // Recursive step on methods declarations
  case TR_METHOD_DECLARATION:
  // Recursive step on arguments
  case TR_ARG_LIST:
  // Recursive step on declarations
  case TR_DECLARATION_LIST:
  // Recursive step on sentences of the block
  case TR_BLOCK:
  // Recursive step on every parameter
  case TR_PARAM_LIST:
  // Recursive step on every sentence
  case TR_SENTENCES_LIST: {
    for (int i = 0; i < node->child_count; i++)
      gen_assembly_code(node->childs[i]);
    return NULL;
  }

  case TR_INVOCATION: {
    for (int i = 0; i < node->child_count; i++)
      gen_assembly_code(node->childs[i]);
    char *t = new_temp();
    printf("CALL %s, %s\n", t, node->info->nombre);
    return t;
  }

  case TR_IF_STATEMENT: {
    char *cond = gen_assembly_code(node->childs[0]);
    char *label_else = new_temp();
    char *label_end = new_temp();
    printf("JZ %s, %s\n", cond, label_else);
    gen_assembly_code(node->childs[1]); // cuerpo if
    printf("JMP %s\n", label_end);
    printf("%s:\n", label_else);
    gen_assembly_code(node->childs[2]); // else
    printf("%s:\n", label_end);
    return NULL;
  }

  case TR_ELSE_BODY:
    if (node->childs[0] != NULL)
      gen_assembly_code(node->childs[0]);
    if (node->childs[1] != NULL)
      gen_assembly_code(node->childs[1]);
    return NULL;

  case TR_WHILE_STATEMENT: {
    char *label_start = new_temp();
    char *label_end = new_temp();
    printf("%s:\n", label_start);
    char *cond = gen_assembly_code(node->childs[0]);
    printf("JZ %s, %s\n", cond, label_end);
    gen_assembly_code(node->childs[1]);
    printf("JMP %s\n", label_start);
    printf("%s:\n", label_end);
    return NULL;
  }

  case TR_RETURN: {
    char *ret = gen_assembly_code(node->childs[0]);
    if (!ret)
      ret = "";
    printf("RET %s\n", ret);
    return NULL;
  }

  case TR_LOGIC_NEGATION: {
    char *val = gen_assembly_code(node->childs[0]);
    char *t = new_temp();
    printf("NOT %s, %s\n", t, val);
    return t;
  }

  case TR_ARITHMETIC_NEGATION: {
    char *val = gen_assembly_code(node->childs[0]);
    char *t = new_temp();
    printf("NEG %s, %s\n", t, val);
    return t;
  }

  // Initial recursive step
  case TR_PROGRAM: {
    printf("; Begin program\n");
    gen_assembly_code(node->childs[0]);
    printf("; End program\n");
    return NULL;
  }

  case TR_EXTERN:
  default:
    return NULL;
  }
}