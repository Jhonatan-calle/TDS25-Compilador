#include "../headers/ast_modules.h"
#include <stdlib.h>

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
  node->child_count = 1;
  node->childs = malloc(sizeof(AST *));
  node->childs[0] = va_arg(args, AST *); // $1: declaration_list, de tipo AST*
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

  AST *exp = va_arg(args, AST *);
  if (exp->info->tVar != tipoIdentificador) {
    fprintf(stderr,
            "<<<<<Error semántico: el identificador '%s' es de tipo '%s' "
            "pero se intenta asignar un valor de tipo '%s'>>>>>\n",
            nombre, tipoDatoToStr(id->tVar), tipoDatoToStr(exp->info->tVar));
    exit(EXIT_FAILURE);
  }
  Simbolo *simbol = malloc(sizeof(Simbolo));
  simbol->tVar = tipoIdentificador; // tipo (enum Tipos)
  simbol->nombre = nombre;          // identificador
  simbol->categoria = S_VAR;
  simbol->valor = exp->info->valor;
  insertar_simbolo(simbol);
  node->info = simbol;
  node->child_count = 1;
  node->childs = malloc(sizeof(AST *));
  node->childs[0] = exp;
}

void module_switch_case_method_declaration(AST *node, va_list args) {
  int tipoIdentificador = va_arg(args, int);
  char *nombre = va_arg(args, char *); // $2: ID, el nombre de la var declarada

  Simbolo *id = buscar_simbolo_local(nombre);
  if (id) {
    fprintf(stderr, "[Error semántico] Identificador '%s' ya declarado.\n",
            nombre);
    exit(EXIT_FAILURE);
  }

  Simbolo *simbol = malloc(sizeof(Simbolo));
  simbol->tVar = tipoIdentificador; // tipo (enum Tipos)
  simbol->nombre = nombre;          // identificador
  simbol->categoria = S_FUNC;
  AST *params = va_arg(args, AST *);
  if (params) {
    simbol->num_params = params->child_count;
    simbol->param_tipos = malloc(sizeof(Tipos) * simbol->num_params);

    for (int i = 0; i < simbol->num_params; i++) {
      simbol->param_tipos[i] = params->childs[i]->info->tVar;
      insertar_simbolo(params->childs[i]->info);
    }
  }
  AST *cuerpo = va_arg(args, AST *);
  // TR_METHOD_DECLARATION que no tiene sentencia reservada Extern
  if (cuerpo->type == TR_BLOCK && tipoIdentificador != T_VOID) {

    int sentencesCount = cuerpo->childs[1]->child_count;
    int returnFound = 0;

    for (int i = 0; i < sentencesCount; i++) {
      AST *sentencia = cuerpo->childs[1]->childs[i];

      if (sentencia->type == TR_RETURN) {
        returnFound = 1;

        if (sentencia->child_count <= 0 || sentencia->childs == NULL ||
            sentencia->childs[0] == NULL) {
          fprintf(stderr,
                  "[Error semántico] En método '%s': 'return' #%d no tiene "
                  "expresión asociada.\n",
                  nombre, i + 1);
          exit(EXIT_FAILURE);
        }

        if (sentencia->childs[0]->info->tVar != tipoIdentificador) {
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
    simbol->cuerpo = cuerpo;
  }
  insertar_simbolo(simbol);
  node->info = simbol;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = params;
  node->childs[1] = cuerpo;
}

void module_switch_case_param(AST *node, va_list args) {
  int tipoIdentificador = va_arg(args, int);
  char *nombre = va_arg(args, char *); // $2: ID, el nombre de la var declarada
  Simbolo *simbol = malloc(sizeof(Simbolo));
  simbol->tVar = tipoIdentificador;
  simbol->nombre = nombre;
  node->info = simbol;
  insertar_simbolo(simbol);
}

void module_switch_case_param_list(AST *node, va_list args) {
  AST *param = va_arg(args, AST *);
  node->child_count = 1;
  node->childs = malloc(sizeof(AST *));
  node->childs[0] = param;
}

void module_switch_case_block(AST *node, va_list args) {
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = va_arg(args, AST *); 
  node->childs[1] = va_arg(args, AST *); // $3: statement_list, de tipo AST*
}

void module_switch_case_asignacion(AST *node, va_list args) {
  char *nombre = va_arg(args, char *); // $1: ID, el nombre de la var a asignar
  exit_if_not_declared(nombre);
  Simbolo *id = buscar_simbolo(nombre);

  AST *exp = va_arg(args, AST *);

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
  if (params != NULL) {
    exit_if_invalid_amount_of_params(params, id, nombre);

    for (int i = 0; i < id->num_params; i++) {
      exit_if_missmatch_types_params_at_invocation(params, id, nombre, i);
    }
    node->child_count = 1;
    node->childs = malloc(sizeof(AST *));
    node->childs[0] = params;
  } else {
    node->child_count = 0;
    node->childs = NULL;
  }

  node->info = id;
}

void module_switch_case_if(AST *node, va_list args) {
  AST *condition = va_arg(args, AST *);
  exit_if_invalid_predicate_type(condition);

  AST *cuerpo = va_arg(args, AST *);
  AST *else_cuerpo = va_arg(args, AST *);
  node->child_count = 3;
  node->childs = malloc(sizeof(AST *) * 3);
  node->childs[0] = condition;
  node->childs[1] = cuerpo;
  node->childs[2] = else_cuerpo;
}


void module_switch_case_else_cuerpo(AST *node, va_list args) {
  AST *declaration_list = va_arg(args, AST *);
  AST *stament_list = va_arg(args, AST *);

  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = declaration_list;
  node->childs[1] = stament_list;
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
  // if (node->child_count != 1) {
  //   return;
  // }
  // node->childs = malloc(sizeof(AST *));
  // node->childs[0] = va_arg(args, AST *);

  // Intentamos leer un posible argumento; el parser debería pasar 1 si tiene
  // expr.
  AST *maybe_expr = va_arg(args, AST *);
  if (maybe_expr != NULL) {
    node->child_count = 1;
    node->childs = malloc(sizeof(AST *));
    node->childs[0] = maybe_expr;
  } else {
    node->child_count = 0;
    node->childs = NULL;
  }
}

void module_switch_case_id(AST *node, va_list args) {
  char *nombre = va_arg(args, char *); // $1: ID, el nombre de la variable
  exit_if_not_declared(nombre);
  Simbolo *id = buscar_simbolo(nombre);

  node->info = id;
  node->child_count = 0;
  node->childs = NULL;
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
  exit_if_binary_arithmetic_operator_mismatch_types(operando1, operando2, op);

  allocate_binary_boolean_node(node, operando1, operando2, op);
}

void module_switch_case_greater_than(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  char *op = ">";
  exit_if_binary_arithmetic_operator_mismatch_types(operando1, operando2, op);

  allocate_binary_boolean_node(node, operando1, operando2, op);
}

void module_switch_case_equal(AST *node, va_list args) {
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  char *op = "==";
  exit_if_operators_mismatch_types(operando1, operando2, op);

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
  node->info->nombre = strdup("TR_VALUE");
  node->info->valor = va_arg(
      args, int); // $1 si es valor numerico, 0 si es false o 1 si es true
  node->child_count = 0;
}

void module_switch_case_arg_list(AST *node, va_list args) {
  node->child_count = 1;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = va_arg(args, AST *);
}
