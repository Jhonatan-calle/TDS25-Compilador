#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/ast.h"

AST *init_node(TipoNodo type, int child_count) {
  AST *node = malloc(sizeof(AST));
  if (!node) {
    fprintf(stderr, "<<<<<Error: no se pudo reservar memoria para AST>>>>>\n");
    exit(EXIT_FAILURE);
  }

  node->type = type;
  node->info = NULL;
  if (child_count > 0) {
    node->childs = malloc(sizeof(AST *) * child_count);
    if (!node->childs) {
      perror("malloc");
      exit(1);
    }
    for (int i = 0; i < child_count; i++)
      node->childs[i] = NULL; // inicializar punteros
  } else {
    node->childs = NULL;
  }
  return node;
}

void debug_print_args(int child_count, va_list args) {
  va_list copy;
  va_copy(copy, args); // make a copy so we don’t consume the original

  printf("[DEBUG] Args:\n");
  for (int i = 0; i < child_count; i++) {
    AST *child = va_arg(copy, AST *);
    printf("  arg[%d] = %p\n", i, (void *)child);
  }

  va_end(copy);
}

void module_switch_case_programa(AST *node, va_list args) {
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] =
      va_arg(args, AST *); // $3: var_declaration_list, de tipo AST*
  node->childs[1] =
      va_arg(args, AST *); // $4: method_declaration_list, de tipo AST*
}

void module_switch_case_var_declaration(AST *node, va_list args) {
  int tipoIdentificador = va_arg(
      args, int); // $1: tipos, el enum de los tipos (internamente un int)
  char *nombre = va_arg(args, char *); // $2: ID, el nombre de la var declarada
  Simbolo *id = buscar_simbolo(nombre);
  if (id) {
    fprintf(stderr, "<<<<<Error: identificador '%s' ya declarado>>>>>\n",
            nombre);
    exit(EXIT_FAILURE);
  }

  AST *exp = va_arg(args, AST *); // $3: expr

  id = crear_simbolo_variable(node, exp, tipoIdentificador, nombre,
                              exp->info->valor);

  if (exp->info->tVar != id->tVar) {
    fprintf(stderr,
            "<<<<<Error semántico: el identificador '%s' es de tipo '%s' "
            "pero se intenta asignar un valor de tipo '%s'>>>>>\n",
            id->nombre, tipoDatoToStr(id->tVar),
            tipoDatoToStr(exp->info->tVar));
    exit(EXIT_FAILURE);
  }
}

void module_switch_case_method_declaration(AST *node, va_list args) {
  int tipoIdentificador = va_arg(
      args, int); // $1: tipos, el enum de los tipos (internamente un int)
  char *nombre = va_arg(args, char *); // $2: ID, el nombre de la var declarada

  Simbolo *id = buscar_simbolo(nombre);
  if (id) {
    fprintf(stderr, "[Error semántico] Identificador '%s' ya declarado.\n",
            nombre);
    exit(EXIT_FAILURE);
  }

  AST *params = va_arg(args, AST *);
  AST *cuerpo = va_arg(args, AST *);

  // TR_METHOD_DECLARATION que no tiene sentencia reservada Extern
  if (node->type == TR_METHOD_DECLARATION) {
    if (tipoIdentificador != T_VOID) {
      // accedo a la parte de sentencias en el bloque
      // la cual es el segundo hijo de un bloque
      AST *sentencias = cuerpo->childs[1];

      int sentencesCount = sentencias->child_count;
      int returnFound = 0;

      for (int i = 0; i < sentencesCount; i++) {
        AST *sentencia = sentencias->childs[i];

        if (sentencia->type == TR_RETURN) {
          returnFound = 1;

          if (sentencia->info->tVar != tipoIdentificador) {
            fprintf(stderr,
                    "[Error semántico] En método '%s': "
                    "el 'return' #%d tiene "
                    "tipo '%s', "
                    "se esperaba '%s'.\n",
                    nombre, i + 1, tipoDatoToStr(sentencia->info->tVar),
                    tipoDatoToStr(tipoIdentificador));
            exit(EXIT_FAILURE);
          }

          // Warning de código inalcanzable
          if (i < sentencesCount - 1) {
            fprintf(stderr,
                    "[Warning semántico] En método '%s': código después del "
                    "'return' #%d es inalcanzable.\n",
                    nombre, i + 1);
          }
        }
      }

      // Error si no hay return en método no-void
      if (!returnFound) {
        fprintf(stderr,
                "[Warning semántico] Método '%s' no tiene un 'return' y es de "
                "tipo no-void.\n",
                nombre);
        exit(EXIT_FAILURE);
      }
    }
  }
  crear_simbolo_metodo(node, params, cuerpo, tipoIdentificador, nombre);
}

void module_switch_case_param(AST *node, va_list args) {
  int tipoIdentificador = va_arg(
      args, int); // $1: tipos, el enum de los tipos (internamente un int)
  char *nombre = va_arg(args, char *); // $2: ID, el nombre de la var declarada
  Simbolo *simbol = malloc(sizeof(Simbolo));
  simbol->tVar = tipoIdentificador;
  simbol->nombre = nombre;
  node->info = simbol;
}

void module_switch_case_param_list(AST *node, va_list args) {
  if (node->child_count <= 0) {
    node->childs = NULL;
    return;
  }

  node->childs = malloc(sizeof(AST *) * node->child_count);
  if (!node->childs) {
    fprintf(stderr,
            "Error: no se pudo reservar memoria para param_list childs\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < node->child_count; i++) {
    AST *child = va_arg(args, AST *);
    node->childs[i] = child;
  }
}

void module_switch_case_block(AST *node, va_list args) {
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] =
      va_arg(args, AST *); // $2: var_declaration_list, de tipo AST*
  node->childs[1] = va_arg(args, AST *); // $3: statement_list, de tipo AST*
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
  aux->type = TR_IDENTIFIER;
  aux->info = id;
  aux->child_count = 0;
  aux->childs = NULL;

  node->childs[0] = aux;
  node->childs[1] = exp;
}

void module_switch_case_invocation(AST *node, va_list args) {
  char *nombre = va_arg(args, char *); // $1: ID, el nombre de la var a asignar
  Simbolo *id = buscar_simbolo(nombre);
  if (!id) {
    fprintf(stderr, "[Error semántico] El método '%s' no está declarado.\n",
            nombre);
    exit(EXIT_FAILURE);
  }

  AST *params = va_arg(args, AST *);
  if (params->child_count != id->num_params) {
    fprintf(stderr,
            "[Error semántico] El método '%s' espera %d parámetro(s), "
            "pero se recibieron %d.\n",
            nombre, id->num_params, params->child_count);
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < id->num_params; i++) {
    if (id->param_tipos[i] != params->childs[i]->info->tVar) {
      fprintf(stderr,
              "[Error semántico] En la llamada a '%s': "
              "el parámetro #%d debería ser de tipo '%s', "
              "pero se encontró '%s'.\n",
              nombre, i + 1,
              tipoDatoToStr(id->param_tipos[i]), // convierte enum Tipo a string
              tipoDatoToStr(params->childs[i]->info->tVar));
      exit(EXIT_FAILURE);
    }
  }
  node->child_count = 1;
  node->info = id;
  node->childs = malloc(sizeof(AST *));
  node->childs[0] = params;
}

void module_switch_case_if(AST *node, va_list args) {
  AST *condition = va_arg(args, AST *);
  if (condition->info->tVar != T_BOOL) {
    fprintf(stderr,
            "[Error semántico] La condición del 'if' debe ser de tipo "
            "'boolean', pero se encontró '%s'.\n",
            tipoDatoToStr(condition->info->tVar));
    exit(EXIT_FAILURE);
  }
  AST *cuerpo = va_arg(args, AST *);

  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = condition;
  node->childs[1] = cuerpo;
}

void module_switch_case_if_else(AST *node, va_list args) {
  AST *condition = va_arg(args, AST *);
  if (condition->info->tVar != T_BOOL) {
    fprintf(stderr,
            "[Error semántico] La condición del 'if' debe ser de tipo "
            "'boolean', pero se encontró '%s'.\n",
            tipoDatoToStr(condition->info->tVar));
    exit(EXIT_FAILURE);
  }
  AST *cuerpo1 = va_arg(args, AST *);
  AST *cuerpo2 = va_arg(args, AST *);

  node->child_count = 3;
  node->childs = malloc(sizeof(AST *) * 3);
  node->childs[0] = condition;
  node->childs[1] = cuerpo1;
  node->childs[2] = cuerpo2;
}

void module_switch_case_while(AST *node, va_list args) {
  AST *condition = va_arg(args, AST *);
  if (condition->info->tVar != T_BOOL) {
    fprintf(stderr,
            "[Error semántico] La condición del 'while' debe ser de tipo "
            "'boolean', pero se encontró '%s'.\n",
            tipoDatoToStr(condition->info->tVar));
    exit(EXIT_FAILURE);
  }
  AST *cuerpo = va_arg(args, AST *);

  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = condition;
  node->childs[1] = cuerpo;
}

void module_switch_case_return(AST *node, va_list args) {
  if (node->child_count != 1) {
    return;
  }
  node->childs = malloc(sizeof(AST *));
  node->childs[0] = va_arg(args, AST *);
}

void module_switch_case_id(AST *node, va_list args) {
  if (node->child_count != 1) {
    return;
  }
  node->childs = malloc(sizeof(AST *));
  node->childs[0] = va_arg(args, AST *);
}

void module_switch_case_negacion_logica(AST *node, va_list args) {
  AST *exp = va_arg(args, AST *);
  if (exp->info->tVar != T_BOOL) {
    fprintf(stderr,
            "[Error semántico] el operador '!' espera una expresion boleana"
            "pero se encontró '%s'.\n",
            tipoDatoToStr(exp->info->tVar));
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = T_BOOL;
  node->info->valor = !exp->info->valor;
  node->info->categoria = exp->info->categoria;
  node->child_count = 1;
  node->childs = malloc(sizeof(AST *));
  node->childs[0] = exp;
}

void module_switch_case_negacion_aritmetica(AST *node, va_list args) {
  AST *exp = va_arg(args, AST *);
  if (exp->info->tVar != T_INT) {
    fprintf(stderr,
            "[Error semántico] el operador '-' espera un entero"
            "pero se encontró '%s'.\n",
            tipoDatoToStr(exp->info->tVar));
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = T_INT;
  node->info->valor = -(exp->info->valor);
  node->child_count = 1;
  node->childs = malloc(sizeof(AST *));
  node->childs[0] = exp;
}

void module_switch_case_suma(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  if (operando1->info->tVar != T_INT || operando2->info->tVar != T_INT) {
    fprintf(stderr, "[Error semántico] el operador '+' espera un entero");
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = T_INT;
  node->info->valor = operando1->info->valor + operando2->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[1] = operando2;
}

void module_switch_case_resta(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  if (operando1->info->tVar != T_INT || operando2->info->tVar != T_INT) {
    fprintf(stderr, "[Error semántico] el operador '-' espera un entero");
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = T_INT;
  node->info->valor = operando1->info->valor - operando2->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[1] = operando2;
}

void module_switch_case_multiplicacion(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  if (operando1->info->tVar != T_INT || operando2->info->tVar != T_INT) {
    fprintf(stderr, "[Error semántico] el operador '*' espera un entero");
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = T_INT;
  node->info->valor = operando1->info->valor * operando2->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[1] = operando2;
}

void module_switch_case_divition(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  if (operando1->info->tVar != T_INT || operando2->info->tVar != T_INT) {
    fprintf(stderr, "[Error semántico] el operador '/' espera un entero");
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = T_INT;
  node->info->valor = operando1->info->valor / operando2->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[1] = operando2;
}

void module_switch_case_modulo(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  if (operando1->info->tVar != T_INT || operando2->info->tVar != T_INT) {
    fprintf(stderr, "[Error semántico] el operador '%%' espera un entero");
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = T_INT;
  node->info->valor = operando1->info->valor % operando2->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[1] = operando2;
}

void module_switch_case_less_than(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  if (operando1->info->tVar != T_INT || operando2->info->tVar != T_INT) {
    fprintf(stderr, "[Error semántico] el operador '<' espera un entero");
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = T_BOOL;
  node->info->valor = operando1->info->valor < operando2->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[1] = operando2;
}

void module_switch_case_greater_than(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  if (operando1->info->tVar != T_INT || operando2->info->tVar != T_INT) {
    fprintf(stderr, "[Error semántico] el operador '>' espera un entero");
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = T_BOOL;
  node->info->valor = operando1->info->valor > operando2->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[1] = operando2;
}

void module_switch_case_equal(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  if (operando1->info->tVar != T_BOOL || operando2->info->tVar != T_BOOL) {
    fprintf(stderr, "[Error semántico] el operador '==' espera boleanos");
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = T_BOOL;
  node->info->valor = operando1->info->valor == operando2->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[1] = operando2;
}

void module_switch_case_and(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  if (operando1->info->tVar != T_BOOL || operando2->info->tVar != T_BOOL) {
    fprintf(stderr, "[Error semántico] el operador '&&' espera boleanos");
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = T_BOOL;
  node->info->valor = operando1->info->valor && operando2->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[1] = operando2;
}

void module_switch_case_or(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  if (operando1->info->tVar != T_BOOL || operando2->info->tVar != T_BOOL) {
    fprintf(stderr, "[Error semántico] el operador '||' espera boleanos");
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = T_BOOL;
  node->info->valor = operando1->info->valor || operando2->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[1] = operando2;
}

void module_switch_case_literal(AST *node, va_list args) {
  node->info = malloc(sizeof(Simbolo));
  node->info->tVar =
      va_arg(args, int); // T_INT o T_BOOL, representado internamente como int
  node->info->nombre = "TR_VALUE";
  node->info->valor = va_arg(
      args, int); // $1 si es valor numerico, 0 si es false o 1 si es true
  node->child_count = 0;
}

AST *new_node(TipoNodo type, int child_count, ...) {
  AST *node = init_node(type, child_count);

  va_list args;
  va_start(args, child_count);

  if (debug_flag) {
    printf("[DEBUG NEW_NODE] Creando nodo: %s, child_count=%d, addr=%p\n",
           tipoNodoToStr(type), child_count, (void *)node);

    for (int i = 0; i < child_count; i++) {
      AST *child = va_arg(args, AST *);
      node->childs[i] = child;
      if (child) {
        printf("  [DEBUG] child[%d] = %s, addr=%p\n", i,
               tipoNodoToStr(child->type), (void *)child);
        if (child->info) {
          printf("    info->tVar=%s, info->valor=%d\n",
                 tipoDatoToStr(child->info->tVar), child->info->valor);
        } else {
          printf("    info=NULL\n");
        }
      } else {
        printf("  [DEBUG] child[%d] = NULL\n", i);
      }
    }
  }

  switch (type) {
  // Nodos de programa
  case TR_PROGRAM:
    module_switch_case_programa(node, args);
    break;

  // Declaraciones de variables
  case TR_VAR_DECLARATION:
    module_switch_case_var_declaration(node, args);
    break;

  // Declaraciones de métodos
  case TR_METHOD_DECLARATION:
  case TR_METHOD_DECLARATION_EXTERN:
    module_switch_case_method_declaration(node, args);
    break;

  // Parámetros
  case TR_PARAM:
    module_switch_case_param(node, args);
    break;

  case TR_PARAM_LIST:
    module_switch_case_param_list(node, args);
    break;

  // Bloques de código
  case TR_BLOCK:
    module_switch_case_block(node, args);
    break;

  // Asignación e invocación
  case TR_ASSIGN:
    module_switch_case_asignacion(node, args);
    break;

  case TR_INVOCATION:
    module_switch_case_invocation(node, args);
    break;

  // Control de flujo
  case TR_IF_STATEMENT:
    module_switch_case_if(node, args);
    break;

  case TR_IF_ELSE_STATEMENT:
    module_switch_case_if_else(node, args);
    break;

  case TR_WHILE_STATEMENT:
    module_switch_case_while(node, args);
    break;

  case TR_RETURN:
    module_switch_case_return(node, args);
    break;

  // Operadores y expresiones
  case TR_LOGIC_NEGATION:
    module_switch_case_negacion_logica(node, args);
    break;

  case TR_ARITHMETIC_NEGATION:
    module_switch_case_negacion_aritmetica(node, args);
    break;

  case TR_ADDITION:
    module_switch_case_suma(node, args);
    break;

  case TR_SUBSTRACTION:
    module_switch_case_resta(node, args);
    break;

  case TR_MULTIPLICATION:
    module_switch_case_multiplicacion(node, args);
    break;

  case TR_DIVITION:
    module_switch_case_divition(node, args);
    break;

  case TR_MODULO:
    module_switch_case_modulo(node, args);
    break;

  case TR_LESS_THAN:
    module_switch_case_less_than(node, args);
    break;

  case TR_GREATER_THAN:
    module_switch_case_greater_than(node, args);
    break;

  case TR_LOGIC_EQUAL:
    module_switch_case_equal(node, args);
    break;

  case TR_AND:
    module_switch_case_and(node, args);
    break;

  case TR_OR:
    module_switch_case_or(node, args);
    break;

  case TR_IDENTIFIER:
    module_switch_case_id(node, args);
    break;

  case TR_VALUE:
    module_switch_case_literal(node, args);
    break;

  // Nodos que son listas de hijos
  case TR_VAR_DECLARATION_LIST:
  case TR_METHOD_DECLARATION_LIST:
  case TR_SENTENCES_LIST:
    if (child_count > 0) {
      node->childs = malloc(sizeof(AST *) * child_count);
      if (!node->childs) {
        fprintf(stderr,
                "Error: no se pudo reservar memoria para lista de hijos\n");
        exit(EXIT_FAILURE);
      }
      for (int i = 0; i < child_count; i++) {
        node->childs[i] = va_arg(args, AST *);
      }
    } else {
      node->childs = NULL;
    }
    break;

  default:
    fprintf(stderr, "Warning: Tipo de nodo no manejado en new_node: %d\n",
            type);
    node->childs = NULL;
    break;
  }

  va_end(args);

  if (debug_flag) {
    printf("[DEBUG NEW_NODE] Nodo %s finalizado, child_count=%d\n",
           tipoNodoToStr(type), node->child_count);
  }

  return node;
}

AST *append_child(AST *list, AST *child) {
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

const char *tipoNodoToStr(TipoNodo t) {
  switch (t) {
  case TR_PROGRAM:
    return "TR_PROGRAM";
  case TR_VAR_DECLARATION:
    return "TR_VAR_DECLARATION";
  case TR_METHOD_DECLARATION:
    return "TR_METHOD_DECLARATION";
  case TR_PARAM:
    return "TR_PARAM";
  case TR_BLOCK:
    return "TR_BLOCK";
  case TR_ASSIGN:
    return "TR_ASSIGN";
  case TR_INVOCATION:
    return "TR_INVOCATION";
  case TR_IF_STATEMENT:
    return "TR_IF_STATEMENT";
  case TR_IF_ELSE_STATEMENT:
    return "TR_IF_ELSE_STATEMENT";
  case TR_WHILE_STATEMENT:
    return "TR_WHILE_STATEMENT";
  case TR_RETURN:
    return "TR_RETURN";
  case TR_IDENTIFIER:
    return "TR_IDENTIFIER";
  case TR_LOGIC_NEGATION:
    return "TR_LOGIC_NEGATION";
  case TR_ARITHMETIC_NEGATION:
    return "TR_ARITHMETIC_NEGATION";
  case TR_ADDITION:
    return "TR_ADDITION";
  case TR_SUBSTRACTION:
    return "TR_SUBSTRACTION";
  case TR_MULTIPLICATION:
    return "TR_MULTIPLICATION";
  case TR_DIVITION:
    return "TR_DIVITION";
  case TR_MODULO:
    return "TR_MODULO";
  case TR_LESS_THAN:
    return "TR_LESS_THAN";
  case TR_GREATER_THAN:
    return "TR_GREATER_THAN";
  case TR_LOGIC_EQUAL:
    return "TR_LOGIC_EQUAL";
  case TR_AND:
    return "TR_AND";
  case TR_OR:
    return "TR_OR";
  case TR_VALUE:
    return "TR_VALUE";
  // add missing ones!
  default:
    return "UNKNOWN";
  }
}
