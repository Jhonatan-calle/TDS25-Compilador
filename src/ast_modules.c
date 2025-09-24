#include "../headers/ast_modules.h"

void allocate_binary_boolean_node(AST *node, AST *operando1, AST *operando2,
                                  char *op) {
  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = T_BOOL;
  set_info_value_depending_operator(node, operando1, operando2, op);
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[1] = operando2;
}

void allocate_binary_integer_node(AST *node, AST *operando1, AST *operando2,
                                  char *op) {
  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = T_INT;
  set_info_value_depending_operator(node, operando1, operando2, op);
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[1] = operando2;
}

void set_info_value_depending_operator(AST *node, AST *operando1,
                                       AST *operando2, char *op) {
  if (strcmp(op, "==") == 0) {
    node->info->valor = operando1->info->valor == operando2->info->valor;
  } else if (strcmp(op, "&&") == 0) {
    node->info->valor = operando1->info->valor && operando2->info->valor;
  } else if (strcmp(op, "||") == 0) {
    node->info->valor = operando1->info->valor || operando2->info->valor;
  } else if (strcmp(op, "<") == 0) {
    node->info->valor = operando1->info->valor < operando2->info->valor;
  } else if (strcmp(op, ">") == 0) {
    node->info->valor = operando1->info->valor > operando2->info->valor;
  } else if (strcmp(op, "+") == 0) {
    node->info->valor = operando1->info->valor + operando2->info->valor;
  } else if (strcmp(op, "-") == 0) {
    node->info->valor = operando1->info->valor - operando2->info->valor;
  } else if (strcmp(op, "*") == 0) {
    node->info->valor = operando1->info->valor * operando2->info->valor;
  } else if (strcmp(op, "/") == 0) {
    node->info->valor = operando1->info->valor / operando2->info->valor;
  } else if (strcmp(op, "%") == 0) {
    node->info->valor = operando1->info->valor % operando2->info->valor;
  } else {
    exit(EXIT_FAILURE);
  }
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
  exit_if_already_declared(nombre);

  AST *exp = va_arg(args, AST *); // $3: expr

  Simbolo *id = crear_simbolo_variable(node, exp, tipoIdentificador, nombre);

  exit_if_types_invalid_at_declaration(exp, id);
}

void module_switch_case_method_declaration(AST *node, va_list args) {
  int tipoIdentificador = va_arg(
      args, int); // $1: tipos, el enum de los tipos (internamente un int)
  char *nombre = va_arg(args, char *); // $2: ID, el nombre de la var declarada

  exit_if_already_declared(nombre);

  AST *params = va_arg(args, AST *);
  AST *cuerpo = va_arg(args, AST *);

  // TR_METHOD_DECLARATION que no tiene sentencia reservada Extern
  if (node->type == TR_METHOD) {
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

          exit_if_invalid_return_type(sentencia, tipoIdentificador, nombre, i);

          warning_if_unreachable_code(i, sentencesCount, nombre);
        }
      }

      error_if_no_return_in_non_void_method(returnFound, nombre);
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
  exit_if_not_declared(nombre);
  Simbolo *id = buscar_simbolo(nombre);

  AST *exp = va_arg(
      args,
      AST *); // $3: expr, lo que se le va asignar a la variable, de tipo AST*

  exit_if_invalid_types_at_assignment(exp, id);

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

  exit_if_not_declared(nombre);

  Simbolo *id = buscar_simbolo(nombre);

  AST *params = va_arg(args, AST *);
  exit_if_invalid_amount_of_params(params, id, nombre);

  for (int i = 0; i < id->num_params; i++) {
    exit_if_missmatch_types_params_at_invocation(params, id, nombre, i);
  }

  node->child_count = 1;
  node->info = id;
  node->childs = malloc(sizeof(AST *));
  node->childs[0] = params;
}

void module_switch_case_if(AST *node, va_list args) {
  AST *condition = va_arg(args, AST *);
  exit_if_invalid_predicate_type(condition);

  AST *cuerpo = va_arg(args, AST *);

  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = condition;
  node->childs[1] = cuerpo;
}

void module_switch_case_if_else(AST *node, va_list args) {
  AST *condition = va_arg(args, AST *);
  exit_if_invalid_predicate_type(condition);

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
  exit_if_invalid_predicate_type(condition);

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

void module_switch_case_negacion_aritmetica(AST *node, va_list args) {
  AST *exp = va_arg(args, AST *);
  exit_if_unary_arithmetic_operator_mismatch_types(exp, "-");

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
  char *op = "+";
  exit_if_binary_arithmetic_operator_mismatch_types(operando1, operando2, op);

  allocate_binary_integer_node(node, operando1, operando2, op);
}

void module_switch_case_resta(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  char *op = "-";
  exit_if_binary_arithmetic_operator_mismatch_types(operando1, operando2, op);

  allocate_binary_integer_node(node, operando1, operando2, op);
}

void module_switch_case_multiplicacion(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  char *op = "*";
  exit_if_binary_arithmetic_operator_mismatch_types(operando1, operando2, op);

  allocate_binary_integer_node(node, operando1, operando2, op);
}

void module_switch_case_divition(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  char *op = "/";
  exit_if_binary_arithmetic_operator_mismatch_types(operando1, operando2, op);

  allocate_binary_integer_node(node, operando1, operando2, op);
}

void module_switch_case_modulo(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  char *op = "%";
  exit_if_binary_arithmetic_operator_mismatch_types(operando1, operando2, op);

  allocate_binary_integer_node(node, operando1, operando2, op);
}

void module_switch_case_negacion_logica(AST *node, va_list args) {
  AST *exp = va_arg(args, AST *);
  exit_if_unary_boolean_operator_mismatch_types(exp, "!");

  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = T_BOOL;
  node->info->valor = !exp->info->valor;
  node->info->categoria = exp->info->categoria;
  node->child_count = 1;
  node->childs = malloc(sizeof(AST *));
  node->childs[0] = exp;
}

void module_switch_case_less_than(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  char *op = "<";
  exit_if_binary_boolean_operator_mismatch_types(operando1, operando2, op);

  allocate_binary_boolean_node(node, operando1, operando2, op);
}

void module_switch_case_greater_than(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  char *op = ">";
  exit_if_binary_boolean_operator_mismatch_types(operando1, operando2, op);

  allocate_binary_boolean_node(node, operando1, operando2, op);
}

void module_switch_case_equal(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  char *op = "==";
  exit_if_binary_boolean_operator_mismatch_types(operando1, operando2, op);

  allocate_binary_boolean_node(node, operando1, operando2, op);
}

void module_switch_case_and(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  char *op = "&&";
  exit_if_binary_boolean_operator_mismatch_types(operando1, operando2, op);

  allocate_binary_boolean_node(node, operando1, operando2, op);
}

void module_switch_case_or(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  char *op = "||";
  exit_if_binary_boolean_operator_mismatch_types(operando1, operando2, op);

  allocate_binary_boolean_node(node, operando1, operando2, op);
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
