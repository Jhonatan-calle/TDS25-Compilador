#include <stdio.h>
#include "../headers/utils.h"

int temp_counter = 0;

/**
 * Assembly util function
 *
 * Called when a new use of a register is used
 * Uses temp_counter global variable to increment or decrement the amount of registers used
 */
char *new_temp()
{
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
char *gen_code(AST *node)
{
  // Case base: Node NULL
  if (!node)
    return NULL;

  switch (node->type)
  {
  // Case base: Leaf
  case TR_VALOR:
  {
    char *t = new_temp();
    printf("LOAD %s, %d\n", t, node->info->valor);
    return t;
  }

  // Case base: Leaf
  case TR_IDENTIFICADOR:
  {
    char *t = new_temp();
    printf("LOAD %s, %s\n", t, node->info->nombre);
    return t;
  }

  // Recursive step on the right side expression
  case TR_ASIGNACION:
  {
    // child[0] = identificador
    // child[1] = expresión
    char *rhs = gen_code(node->childs[1]);
    printf("STORE %s, %s\n", node->childs[0]->info->nombre, rhs);
    return rhs;
  }

  // Recursive step on both sides
  case TR_SUMA:
  {
    char *lhs = gen_code(node->childs[0]);
    char *rhs = gen_code(node->childs[1]);
    char *t = new_temp();
    printf("ADD %s, %s, %s\n", t, lhs, rhs);
    return t;
  }

  // Recursive step on both sides
  case TR_MULTIPLICACION:
  {
    char *lhs = gen_code(node->childs[0]);
    char *rhs = gen_code(node->childs[1]);
    char *t = new_temp();
    printf("MUL %s, %s, %s\n", t, lhs, rhs);
    return t;
  }

  // Recursive step on both sides
  case TR_AND:
  {
    char *lhs = gen_code(node->childs[0]);
    char *rhs = gen_code(node->childs[1]);
    char *t = new_temp();
    printf("AND %s, %s, %s\n", t, lhs, rhs);
    return t;
  }

  // Recursive step on both sides
  case TR_OR:
  {
    char *lhs = gen_code(node->childs[0]);
    char *rhs = gen_code(node->childs[1]);
    char *t = new_temp();
    printf("OR %s, %s, %s\n", t, lhs, rhs);
    return t;
  }

  // Recursive step on every sentence
  case TR_LISTA_SENTENCIAS:
  {
    for (int i = 0; i < node->child_count; i++)
      gen_code(node->childs[i]);
    return NULL;
  }

  // Initial recursive step
  case TR_PROGRAMA:
  {
    printf("; Begin program\n");
    gen_code(node->childs[0]);
    printf("; End program\n");
    return NULL;
  }

  default:
    return NULL;
  }
}

/**
 * Print util function
 *
 * Default print if global debug flag is enabled
 */
void print_if_debug_flag(char *str)
{
  if (debug_flag)
    printf("%s\n", str);
}

/**
 * Print util function
 *
 * Calls gen_code function if global assembly flag is enabled
 */
void gen_assembly_if_assembly_flag(AST *root)
{
  if (assembly_flag)
    gen_code(root);
}

/**
 * Utility function to process arguments
 *
 * Receive variables from main and processes them
 * Returns -1 if error, otherwise returns 0
 */
int process_arguments(int argc, char *argv[], char **outfile, char **target, char **opt, char **inputfile)
{
  static char outfile_with_ext[256]; // Static to ensure it persists after function returns
  *target = NULL;
  *opt = NULL;
  *outfile = "a.out";
  *inputfile = NULL;

  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "-output") == 0)
    {
      if (i + 1 < argc)
      {
        snprintf(outfile_with_ext, sizeof(outfile_with_ext), "%s.out", argv[i + 1]);
        *outfile = outfile_with_ext;
        i++;
      }
      else
      {
        fprintf(stderr, "Error: missing argument after -o\n");
        return -1;
      }
    }
    else if (strcmp(argv[i], "-target") == 0 || strcmp(argv[i], "-t") == 0)
    {
      if (i + 1 < argc)
      {
        *target = argv[++i];
      }
      else
      {
        fprintf(stderr, "Error: missing argument after -target\n");
        return -1;
      }
    }
    else if (strcmp(argv[i], "-opt") == 0)
    {
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        *opt = argv[++i];
      }
      else
      {
        *opt = "all";
      }
    }
    else if (strcmp(argv[i], "-debug") == 0 || strcmp(argv[i], "-d") == 0)
    {
      debug_flag = 1;
    }
    else
    {
      *inputfile = argv[i];
    }
  }

  if (!*inputfile)
  {
    return -1;
  }

  return 0;
}