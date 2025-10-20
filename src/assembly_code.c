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
void gen_assembly_code(AST *node) {
  // Case base: Node
  if (!node)
    return;

  switch (node->type) {
  // Case base: Leaf
  case TR_VALUE: {
    // char *t = new_temp();
    printf("  mov $%d ", node->info->valor);
    // return t;
  }

  // Case base: Leaf
  case TR_IDENTIFIER: {
    // char *t = new_temp();
    printf("%%(%p)\n", node->info);
    // return t;
  }

  case TR_VAR_DECLARATION: {
    if (node->child_count > 0 && node->childs[0])
      gen_assembly_code(node->childs[0]);
    return;
  }

  // Recursive step on the right side expression
  case TR_ASSIGN: {
    // child[0] = identificador
    // child[1] = expresión
    printf("  mov %%%p ", node->childs[0]->info);
    gen_assembly_code(node->childs[1]);
  }

  // Recursive step on both sides
  case TR_ADDITION: {
    char *op1 = NULL;
    char *op2 = NULL;
    if (node->childs[0]->type == TR_IDENTIFIER) {
      op1 = node->childs[0]->info->nombre;
    }
    if (node->childs[1]->type == TR_IDENTIFIER) {
      op2 = node->childs[1]->info->nombre;
    }
    if (op1) {
      printf("  mov %%(%p) %%(eax)\n", node->childs[0]->info);
    }
    if (op2) {
      printf("  mov %%(%p) %%(edx)\n", node->childs[1]->info);
    }
    printf("  add %%(eax) %%(edx)\n");
    // gen_assembly_code(node->childs[0]);
    // gen_assembly_code(node->childs[1]);
    return;
  }

  case TR_SUBSTRACTION: {
    printf("  sub ");
    gen_assembly_code(node->childs[0]);
    gen_assembly_code(node->childs[1]);
    return;
  }

  // Recursive step on both sides
  case TR_MULTIPLICATION: {
    printf("  mov 1 ");
    gen_assembly_code(node->childs[0]);
    gen_assembly_code(node->childs[1]);
    return;
  }

  case TR_DIVITION: {
    printf("  mov -1 ");
    gen_assembly_code(node->childs[0]);
    gen_assembly_code(node->childs[1]);
    return;
  }

  case TR_MODULO: {
    printf("  mod ");
    gen_assembly_code(node->childs[0]);
    gen_assembly_code(node->childs[1]);
    return;
  }

  case TR_LESS_THAN:
  case TR_GREATER_THAN:
  case TR_LOGIC_EQUAL: {
    char *op = (node->type == TR_LESS_THAN)      ? "jl"
               : (node->type == TR_GREATER_THAN) ? "jge"
                                                 : "je";
    printf("  %s", op);
    gen_assembly_code(node->childs[0]);
    gen_assembly_code(node->childs[1]);
    return;
  }

  // Recursive step on both sides
  case TR_AND: {
    printf("  and ");
    gen_assembly_code(node->childs[0]);
    gen_assembly_code(node->childs[1]);
    return;
  }

  // Recursive step on both sides
  case TR_OR: {
    printf("  or ");
    gen_assembly_code(node->childs[0]);
    gen_assembly_code(node->childs[1]);
    return;
  }

  // Parameter
  case TR_PARAM:
    if (node->child_count > 0 && node->childs[0])
      gen_assembly_code(node->childs[0]);
    return;

  // Recursive step on methods declarations
  case TR_METHOD_DECLARATION:
    printf("%s: \n", node->info->nombre);
    printf("  enter\n");
    for (int i = 0; i < node->child_count; i++)
      gen_assembly_code(node->childs[i]);
    return;
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
    return;
  }

  case TR_INVOCATION: {
    printf("  call ");
    for (int i = 0; i < node->child_count; i++)
      gen_assembly_code(node->childs[i]);
    return;
  }

  case TR_IF_STATEMENT: {
    printf("  jz ");
    gen_assembly_code(node->childs[0]);
    printf("  jmp ");
    gen_assembly_code(node->childs[1]);
    // printf("%s:\n", label_else);
    gen_assembly_code(node->childs[2]); // else
    // printf("%s:\n", label_end);
    return;
  }

  case TR_ELSE_BODY:
    if (node->childs[0] != NULL)
      gen_assembly_code(node->childs[0]);
    if (node->childs[1] != NULL)
      gen_assembly_code(node->childs[1]);
    return;

  case TR_WHILE_STATEMENT: {
    printf("\n");
    printf("  jz ");
    gen_assembly_code(node->childs[0]);
    printf("  jmp ");
    gen_assembly_code(node->childs[1]);
    printf("\n");
    return;
  }

  case TR_RETURN: {
    printf("  leave\n");
    printf("  ret\n");
    gen_assembly_code(node->childs[0]);
    return;
  }

  case TR_LOGIC_NEGATION: {
    printf("  not");
    gen_assembly_code(node->childs[0]);
    return;
  }

  case TR_ARITHMETIC_NEGATION: {
    printf("  imul $-1 ");
    gen_assembly_code(node->childs[0]);
    return;
  }

  // Initial recursive step
  case TR_PROGRAM: {
    printf("program:\n");
    gen_assembly_code(node->childs[0]);
    return;
  }

  case TR_EXTERN:
  default:
    return;
  }
}