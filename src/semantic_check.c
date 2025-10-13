#include "../headers/semantic_check.h"

void exit_if_already_declared(char *nombre) {
  Simbolo *id = buscar_simbolo_local(nombre);
  if (id) {
    fprintf(stderr, "<<<<<Error: identificador '%s' ya declarado>>>>>\n",
            nombre);
    exit(EXIT_FAILURE);
  }
}

void exit_if_already_declared_locally(char *nombre) {
  Simbolo *id = buscar_simbolo_local(nombre);
  if (id) {
    fprintf(stderr, "[Error semántico] Identificador '%s' ya declarado.\n",
            nombre);
    exit(EXIT_FAILURE);
  }
}

void exit_if_not_declared(char *nombre) {
  Simbolo *id = buscar_simbolo(nombre);
  if (!id)
    id = buscar_simbolo_local(nombre);
  if (!id) {
    fprintf(stderr, "<<<<<Error: identificador '%s' no declarado>>>>>\n",
            nombre);
    exit(EXIT_FAILURE);
  }
}

void exit_if_types_invalid_at_declaration(AST *exp, Tipos tipoIdentificador,
                                          char *nombre) {
  if (exp->info->tVar != tipoIdentificador) {
    fprintf(stderr,
            "<<<<<Error semántico: el identificador '%s' es de tipo '%s' "
            "pero se intenta asignar un valor de tipo '%s'>>>>>\n",
            nombre, tipoDatoToStr(tipoIdentificador),
            tipoDatoToStr(exp->info->tVar));
    exit(EXIT_FAILURE);
  }
}

void exit_if_invalid_return_type(AST *sentencia, int tipoIdentificador,
                                 char *nombre, int i) {
  // Si no hay expresión en el return (info == NULL)
  if (sentencia->child_count == 0) {
    if (tipoIdentificador != T_VOID) {
      fprintf(stderr,
              "Error semántico: la función '%s' (de tipo %s) tiene un `return` "
              "sin expresión en la sentencia %d.\n",
              nombre, tipoDatoToStr(tipoIdentificador), i + 1);
      exit(EXIT_FAILURE);
    } else {
      // return sin expresión en función void -> válido
      return;
    }
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
}

void warning_if_unreachable_code(int i, int sentencesCount, char *nombre) {
  // Warning de código inalcanzable
  if (i < sentencesCount - 1) {
    fprintf(stderr,
            "[Warning semántico] En método '%s': código después del "
            "'return' #%d es inalcanzable.\n",
            nombre, i + 1);
  }
}

void exit_if_no_return_in_non_void_method(int returnFound, char *nombre) {
  // Error si no hay return en método no-void
  if (!returnFound) {
    fprintf(stderr,
            "[Error semántico] Método '%s' no tiene un 'return' y es de "
            "tipo no-void.\n",
            nombre);
    exit(EXIT_FAILURE);
  }
}

void exit_if_invalid_types_at_assignment(AST *exp, Simbolo *id) {
  if (exp->info->tVar != id->tVar) {
    fprintf(stderr,
            "<<<<<Error semántico: el identificador '%s' es de tipo '%s' "
            "pero se intenta asignar un valor de tipo '%s'>>>>>\n",
            id->nombre, tipoDatoToStr(id->tVar),
            tipoDatoToStr(exp->info->tVar));
    exit(EXIT_FAILURE);
  }
}

void exit_if_invalid_amount_of_params(AST *params, Simbolo *id, char *nombre) {
  if (params->child_count != id->num_params) {
    fprintf(stderr,
            "[Error semántico] El método '%s' espera %d parámetro(s), "
            "pero se recibieron %d.\n",
            nombre, id->num_params, params->child_count);
    exit(EXIT_FAILURE);
  }
}

void exit_if_missmatch_types_params_at_invocation(AST *params, Simbolo *id,
                                                  char *nombre, int i) {
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

void exit_if_invalid_predicate_type(AST *condition) {
  if (condition->info->tVar != T_BOOL) {
    fprintf(stderr,
            "[Error semántico] La condición debe ser de tipo "
            "'boolean', pero se encontró '%s'.\n",
            tipoDatoToStr(condition->info->tVar));
    exit(EXIT_FAILURE);
  }
}

void exit_if_unary_arithmetic_operator_mismatch_types(AST *exp, char *op) {
  if (exp->info->tVar != T_INT) {
    fprintf(stderr,
            "[Error semántico] el operador '%s' espera un entero "
            "pero se encontró '%s'.\n",
            op, tipoDatoToStr(exp->info->tVar));
    exit(EXIT_FAILURE);
  }
}

void exit_if_binary_arithmetic_operator_mismatch_types(AST *operando1,
                                                       AST *operando2,
                                                       char *op) {
  if (operando1->info->tVar != T_INT || operando2->info->tVar != T_INT) {
    fprintf(stderr, "[Error semántico] el operador '%s' espera dos enteros",
            op);
    exit(EXIT_FAILURE);
  }
}

void exit_if_unary_boolean_operator_mismatch_types(AST *exp, char *op) {
  if (exp->info->tVar != T_BOOL) {
    fprintf(stderr,
            "[Error semántico] el operador '%s' espera una expresion boleana "
            "pero se encontró '%s'.\n",
            op, tipoDatoToStr(exp->info->tVar));
    exit(EXIT_FAILURE);
  }
}

void exit_if_binary_boolean_operator_mismatch_types(AST *operando1,
                                                    AST *operando2, char *op) {
  if (operando1->info->tVar != T_BOOL || operando2->info->tVar != T_BOOL) {
    fprintf(stderr, "[Error semántico] el operador '%s' espera boleanos", op);
    exit(EXIT_FAILURE);
  }
}

void exit_if_operators_mismatch_types(AST *operando1, AST *operando2,
                                      char *op) {
  if (operando1->info->tVar != operando2->info->tVar) {
    fprintf(
        stderr,
        "[Error semántico] el operador '%s' espera operandos del mismo tipo",
        op);
    exit(EXIT_FAILURE);
  }
}

void  exit_if_return_with_no_expression(AST *sentencia, char *nombre, int i) {
  if (sentencia->child_count <= 0 || sentencia->childs == NULL ||
      sentencia->childs[0] == NULL) {
    fprintf(stderr,
            "[Error semántico] En método '%s': 'return' #%d no tiene "
            "expresión asociada.\n",
            nombre, i + 1);
    exit(EXIT_FAILURE);
  }
}