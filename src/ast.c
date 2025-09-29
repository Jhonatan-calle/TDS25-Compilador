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

AST *new_node(TipoNodo type, int child_count, ...) {
  AST *node = init_node(type, child_count);

  va_list args;
  va_start(args, child_count);

  // if (debug_flag) {
  //   printf("[DEBUG NEW_NODE] Creando nodo: %s, child_count=%d, addr=%p\n",
  //          tipoNodoToStr(type), child_count, (void *)node);

  //   for (int i = 0; i < child_count; i++) {
  //     AST *child = va_arg(args, AST *);
  //     node->childs[i] = child;
  //     if (child) {
  //       printf("  [DEBUG] child[%d] = %s, addr=%p\n", i,
  //              tipoNodoToStr(child->type), (void *)child);
  //       if (child->info) {
  //         printf("    info->tVar=%s, info->valor=%d\n",
  //                tipoDatoToStr(child->info->tVar), child->info->valor);
  //       } else {
  //         printf("    info=NULL\n");
  //       }
  //     } else {
  //       printf("  [DEBUG] child[%d] = NULL\n", i);
  //     }
  //   }
  // }

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

  case TR_ELSE_BODY:
    module_switch_case_else_cuerpo(node, args);
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
  case TR_ARG_LIST:
    module_switch_case_arg_list(node, args);
    break;
  case TR_SENTENCES_LIST:
  case TR_DECLARATION_LIST:
  case TR_EXTERN:
    // char *str = strcat("Pass Case: ", tipoNodoToStr(type));
    // print_if_debug_flag(str);
    break;
  default:
    fprintf(stderr, "Warning: Tipo de nodo no manejado en new_node: %s\n",
            tipoNodoToStr(type));
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
