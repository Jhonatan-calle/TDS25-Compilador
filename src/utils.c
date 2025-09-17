#include <stdio.h>
#include "../headers/utils.h"
#include "../headers/simbolos.h"

int temp_counter = 0;

char *new_temp()
{
  char buffer[16];
  snprintf(buffer, sizeof(buffer), "t%d", temp_counter++);
  return strdup(buffer);
}

char *gen_code(AST *node)
{
  if (!node)
    return NULL;

  switch (node->type)
  {
  case TR_VALOR:
  {
    char *t = new_temp();
    printf("LOAD %s, %d\n", t, node->info->valor);
    return t;
  }

  case TR_IDENTIFICADOR:
  {
    char *t = new_temp();
    printf("LOAD %s, %s\n", t, node->info->nombre);
    return t;
  }

  case TR_ASIGNACION:
  {
    // child[0] = identificador
    // child[1] = expresión
    char *rhs = gen_code(node->childs[1]);
    printf("STORE %s, %s\n", node->childs[0]->info->nombre, rhs);
    return rhs;
  }

  case TR_SUMA:
  {
    char *lhs = gen_code(node->childs[0]);
    char *rhs = gen_code(node->childs[1]);
    char *t = new_temp();
    printf("ADD %s, %s, %s\n", t, lhs, rhs);
    return t;
  }

  case TR_MULTIPLICACION:
  {
    char *lhs = gen_code(node->childs[0]);
    char *rhs = gen_code(node->childs[1]);
    char *t = new_temp();
    printf("MUL %s, %s, %s\n", t, lhs, rhs);
    return t;
  }

  case TR_AND:
  {
    char *lhs = gen_code(node->childs[0]);
    char *rhs = gen_code(node->childs[1]);
    char *t = new_temp();
    printf("AND %s, %s, %s\n", t, lhs, rhs);
    return t;
  }

  case TR_OR:
  {
    char *lhs = gen_code(node->childs[0]);
    char *rhs = gen_code(node->childs[1]);
    char *t = new_temp();
    printf("OR %s, %s, %s\n", t, lhs, rhs);
    return t;
  }

  case TR_LISTA_SENTENCIAS:
  {
    for (int i = 0; i < node->child_count; i++)
      gen_code(node->childs[i]);
    return NULL;
  }

  case TR_PROGRAMA:
  {
    printf("; inicio programa\n");
    gen_code(node->childs[0]);
    printf("; fin programa\n");
    return NULL;
  }

  default:
    return NULL;
  }
}
