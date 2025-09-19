#include "../headers/ast.h"
#include "../headers/simbolos.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_indent(int level) {
  for (int i = 0; i < level; i++)
    printf("  "); // 2 espacios por nivel
}

void print_ast(AST *node, int level) {
  if (!node)
    return;

  print_indent(level);
  printf("%s", tipoNodoToStr(node->type));

  switch (node->type) {
  case TR_PROGRAMA:
    printf(" [tipo retorno: %s]", tipoDatoToStr(node->info->tVar));
    break;
  case TR_DECLARACION:
    printf(" [tipo: %s, id: %s]", tipoDatoToStr(node->info->tVar),
           node->info->nombre);
    break;
  case TR_ASIGNACION:
    if (node->childs && node->childs[0] && node->childs[0]->info)
      printf(" [asigna a id: %s]", node->childs[0]->info->nombre);
    break;
  case TR_VALOR:
    printf(" [valor: %d, tipo: %s]", node->info->valor,
           tipoDatoToStr(node->info->tVar));
    break;
  case TR_IDENTIFICADOR:
    if (node->info)
      printf(" [id: %s, tipo: %s]", node->info->nombre,
             tipoDatoToStr(node->info->tVar));
    break;
  case TR_SUMA:
    printf(" [+], valor: %d]", node->info->valor);
    break;
  case TR_MULTIPLICACION:
    printf(" [*], valor: %d]", node->info->valor);
    break;
  case TR_LISTA_SENTENCIAS:
    printf(" [lista de sentencias]");
    break;
  case TR_RETURN:
    printf(" [return]");
    break;
  case TR_AND:
    printf(" [&&], valor: %d]", node->info->valor);
    break;
  case TR_OR:
    printf(" [||], valor: %d]", node->info->valor);
    break;
  default:
    printf(" [?]");
    break;
  }

  printf("\n");

  // Recursión en hijos
  for (int i = 0; i < node->child_count; i++)
    print_ast(node->childs[i], level + 1);
}

AST *init_node(TipoNodo type, int child_count) {
  AST *node = malloc(sizeof(AST));
  if (!node) {
    fprintf(stderr, "<<<<<Error: no se pudo reservar memoria para AST>>>>>\n");
    exit(EXIT_FAILURE);
  }

  node->type = type;
  node->info = NULL;
  node->child_count = child_count;
  node->childs = NULL;
  return node;
}

void module_switch_case_programa(AST *node, va_list args) {
  node->info = malloc(sizeof(Simbolo));
  node->info->nombre = strdup("programa");
  node->info->tVar =
      va_arg(args, int); // $1: tiposF, tipo de retorno del programa/funcion
  node->child_count = 1;
  node->childs = malloc(sizeof(AST *));
  node->childs[0] = va_arg(args, AST *); // $6: lista_sentencias, de tipo AST*
}

void module_switch_case_declaracion(AST *node, va_list args) {
  int tipoIdentificador = va_arg(
      args, int); // $1: tipos, el enum de los tipos (internamente un int)
  char *nombre = va_arg(args, char *); // $2: ID, el nombre de la var declarada
  Simbolo *id = buscar_simbolo(nombre);
  if (id) {
    fprintf(stderr, "<<<<<Error: identificador '%s' ya declarado>>>>>\n",
            nombre);
    exit(EXIT_FAILURE);
  }
  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = tipoIdentificador; // tipo (enum Tipos)
  node->info->nombre = nombre;          // identificador
  insertar_simbolo(node->info);
  node->child_count = 0;
}

void module_switch_case_asignacion(AST *node, va_list args) {
  char *nombre = va_arg(args, char *); // $1: ID, el nombre de la var a asignar
  Simbolo *id = buscar_simbolo(nombre);
  if (!id) {
    fprintf(stderr, "<<<<<Error: identificador '%s' no declarado>>>>>\n",
            nombre);
    exit(EXIT_FAILURE);
  }

  AST *exp = va_arg(
      args,
      AST *); // $3: expr, lo que se le va asignar a la variable, de tipo AST*

  if (exp->info->tVar != id->tVar) {
    fprintf(stderr,
            "<<<<<Error semántico: el identificador '%s' es de tipo '%s' "
            "pero se intenta asignar un valor de tipo '%s'>>>>>\n",
            id->nombre, tipoDatoToStr(id->tVar),
            tipoDatoToStr(exp->info->tVar));
    exit(EXIT_FAILURE);
  }
  id->valor = exp->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);

  AST *aux = malloc(sizeof(AST));
  aux->type = TR_IDENTIFICADOR;
  aux->info = id;
  aux->child_count = 0;
  aux->childs = NULL;

  node->childs[0] = aux;
  node->childs[1] = exp;
}

void module_switch_case_valor(AST *node, va_list args) {
  node->info = malloc(sizeof(Simbolo));
  node->info->tVar =
      va_arg(args, int); // T_INT o T_BOOL, representado internamente como int
  node->info->nombre = strdup("TR_VALOR");
  node->info->valor = va_arg(
      args, int); // $1 si es valor numerico, 0 si es false o 1 si es true
  node->child_count = 0;
}

void module_switch_case_identificador(AST *node, va_list args) {
  char *nombre = va_arg(args, char *); // $1: ID, el nombre de la variable
  Simbolo *id = buscar_simbolo(nombre);
  if (!id) {
    fprintf(stderr, "<<<<<Error: identificador '%s' no declarado>>>>>\n",
            nombre);
    exit(EXIT_FAILURE);
  }
  node->info = id;
  node->child_count = 0;
  node->childs = NULL;
}

void module_switch_case_suma(AST *node, va_list args) {
  AST *op1 = va_arg(args, AST *); // $1: expr, el primer operando
  AST *op2 = va_arg(args, AST *); // $3: expr, el segundo operando
  if (op1->info->tVar != T_INT || op2->info->tVar != T_INT) {
    fprintf(stderr, "operacion con tipos invalidos\n");
    exit(EXIT_FAILURE);
  }
  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = T_INT;
  node->info->nombre = strdup("SUMA");

  // operacion principal: SUMA
  node->info->valor = op1->info->valor + op2->info->valor;

  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);

  node->childs[0] = op1;
  node->childs[1] = op2;
}

void module_switch_case_multiplicacion(AST *node, va_list args) {
  AST *op1 = va_arg(args, AST *); // $1: expr, el primer operando
  AST *op2 = va_arg(args, AST *); // $3: expr, el segundo operando
  if (op1->info->tVar != T_INT || op2->info->tVar != T_INT) {
    fprintf(stderr, "operacion con tipos invalidos\n");
    exit(EXIT_FAILURE);
  }
  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = T_INT;
  node->info->nombre = strdup("MULTIPLICACION");

  // operacion principal: MULTIPLICACION
  node->info->valor = op1->info->valor * op2->info->valor;

  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);

  node->childs[0] = op1;
  node->childs[1] = op2;
}

void module_switch_case_and(AST *node, va_list args) {
  AST *op1 = va_arg(args, AST *); // $1: expr, el primer operando
  AST *op2 = va_arg(args, AST *); // $3: expr, el segundo operando
  if (op1->info->tVar != T_BOOL || op2->info->tVar != T_BOOL) {
    fprintf(stderr, "operacion con tipos invalidos\n");
    exit(EXIT_FAILURE);
  }
  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = T_BOOL;
  node->info->nombre = strdup("CONJUNCION");

  // operacion principal: AND
  node->info->valor = (op1->info->valor != 0) && (op2->info->valor != 0);

  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);

  node->childs[0] = op1;
  node->childs[1] = op2;
}

void module_switch_case_or(AST *node, va_list args) {
  AST *op1 = va_arg(args, AST *); // $1: expr, el primer operando
  AST *op2 = va_arg(args, AST *); // $3: expr, el segundo operando
  if (op1->info->tVar != T_BOOL || op2->info->tVar != T_BOOL) {
    fprintf(stderr, "operacion con tipos invalidos\n");
    exit(EXIT_FAILURE);
  }
  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = T_BOOL;
  node->info->nombre = strdup("DISYUNCION");

  // operacion principal: OR
  node->info->valor = (op1->info->valor != 0) || (op2->info->valor != 0);

  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);

  node->childs[0] = op1;
  node->childs[1] = op2;
}

void module_switch_case_return(AST *node, int child_count, va_list args) {
  node->childs = malloc(sizeof(AST *) * child_count);
  for (int i = 0; i < child_count; i++)
    node->childs[i] =
        va_arg(args, AST *); // $2: expr, AST* que se retorna de la funcion
}

AST *new_node(TipoNodo type, int child_count, ...) {
  AST *node = init_node(type, child_count);

  va_list args;
  va_start(args, child_count);

  switch (type) {
  case TR_PROGRAMA:
    module_switch_case_programa(node, args);
    break;
  case TR_DECLARACION:
    module_switch_case_declaracion(node, args);
    break;
  case TR_ASIGNACION:
    module_switch_case_asignacion(node, args);
    break;
  case TR_VALOR:
    module_switch_case_valor(node, args);
    break;
  case TR_IDENTIFICADOR:
    module_switch_case_identificador(node, args);
    break;
  case TR_SUMA:
    module_switch_case_suma(node, args);
    break;
  case TR_MULTIPLICACION:
    module_switch_case_multiplicacion(node, args);
    break;
  case TR_AND:
    module_switch_case_and(node, args);
    break;
  case TR_OR:
    module_switch_case_or(node, args);
    break;
  case TR_RETURN:
    module_switch_case_return(node, child_count, args);
    break;
  // TR_PERFIL, TR_EXPRESION
  default:
    break;
  }

  va_end(args);
  return node;
}

AST *append_child(AST *list, AST *child) {
  if (!list)
    return new_node(TR_LISTA_SENTENCIAS, 1, child);

  list->childs = realloc(list->childs, sizeof(AST *) * (list->child_count + 1));
  if (!list->childs) {
    fprintf(stderr, "Error realloc en append_child\n");
    exit(EXIT_FAILURE);
  }
  list->childs[list->child_count] = child;
  list->child_count += 1;

  return list;
}

void free_ast(AST *node) {
  if (!node)
    return;

  for (int i = 0; i < node->child_count; i++)
    free_ast(node->childs[i]);
  free(node->childs);
  free(node);
}

const char *tipoNodoToStr(TipoNodo type) {
  switch (type) {
  case TR_PROGRAMA:
    return "PROGRAMA";
  case TR_METHOD:
    return "PERFIL";
  case TR_DECLARACION:
    return "DECLARACION";
  case TR_ASIGNACION:
    return "ASIGNACION";
  case TR_RETURN:
    return "RETURN";
  case TR_VALOR:
    return "VALOR";
  case TR_IDENTIFICADOR:
    return "IDENTIFICADOR";
  case TR_SUMA:
    return "SUMA";
  case TR_MULTIPLICACION:
    return "MULTIPLICACION";
  case TR_LISTA_SENTENCIAS:
    return "LISTA_SENTENCIAS";
  case TR_AND:
    return "CONJUNCION";
  case TR_OR:
    return "DISYUNCION";
  default:
    return "DESCONOCIDO";
  }
}

const char *tipoDatoToStr(Tipos type) {
  switch (type) {
  case T_INT:
    return "INT";
  case T_BOOL:
    return "BOOL";
  case T_VOID:
    return "VOID";
  default:
    return "DESCONOCIDO";
  }
}
