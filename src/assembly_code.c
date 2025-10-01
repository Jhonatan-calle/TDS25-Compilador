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

  // Recursive step on both sides
  case TR_MULTIPLICATION: {
    char *lhs = gen_assembly_code(node->childs[0]);
    char *rhs = gen_assembly_code(node->childs[1]);
    char *t = new_temp();
    printf("MUL %s, %s, %s\n", t, lhs, rhs);
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

  // Recursive step on every sentence
  case TR_SENTENCES_LIST: {
    for (int i = 0; i < node->child_count; i++)
      gen_assembly_code(node->childs[i]);
    return NULL;
  }

  // Initial recursive step
  case TR_PROGRAM: {
    printf("; Begin program\n");
    gen_assembly_code(node->childs[0]);
    printf("; End program\n");
    return NULL;
  }

  default:
    return NULL;
  }
}