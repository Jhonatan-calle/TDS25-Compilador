#include "../headers/semantic_check.h"

void exit_if_already_declared(char *name) {
  Symbol *id = search_symbol_locally(name);
  if (id) {
    fprintf(stderr, "[Semantic error]: Identifier '%s' already declared.\n",
            name);
    exit(EXIT_FAILURE);
  }
}

void exit_if_not_declared(char *name) {
  Symbol *id = search_symbol_globally(name);
  if (!id)
    id = search_symbol_locally(name);
  if (!id) {
    fprintf(stderr, "[Semantic error]: Identifier '%s' not declared.\n", name);
    exit(EXIT_FAILURE);
  }
}

void exit_if_types_invalid_at_declaration(AST *exp, Types type_identifier,
                                          char *name) {
  if (exp->info->t_var != type_identifier) {
    fprintf(stderr,
            "[Semantic error]: the identifier '%s' is of type '%s' "
            "but a value of type '%s' is being assigned.\n",
            name, data_types_to_string(type_identifier),
            data_types_to_string(exp->info->t_var));
    exit(EXIT_FAILURE);
  }
}

void exit_if_invalid_return_type(AST *sentence, int type_identifier, char *name,
                                 int i) {
  // If there is no expression in the return (info == NULL)
  if (sentence->child_count == 0) {
    if (type_identifier != T_VOID) {
      fprintf(stderr,
              "[Semantic error]: the function '%s' (of type %s) has a `return` "
              "without expression in the sentence %d.\n",
              name, data_types_to_string(type_identifier), i + 1);
      exit(EXIT_FAILURE);
    } else {
      // return without expression in void function -> valid
      return;
    }
  }

  if (sentence->children[0]->info->t_var != type_identifier) {
    fprintf(stderr,
            "[Semantic error]: In method '%s': "
            "the 'return' #%d has "
            "type '%s', "
            "expected '%s'.\n",
            name, i + 1, data_types_to_string(sentence->info->t_var),
            data_types_to_string(type_identifier));
    exit(EXIT_FAILURE);
  }
}

void warning_if_unreachable_code(int i, int sentences_count, char *name) {
  // Warning of unreachable code
  if (i < sentences_count - 1) {
    fprintf(stderr,
            "[Semantic warning]: In method '%s': code after "
            "'return' #%d is unreachable.\n",
            name, i + 1);
  }
}

void exit_if_no_return_in_non_void_method(int return_found, char *name) {
  // Error if there is no return in non-void method
  if (!return_found) {
    fprintf(stderr,
            "[Semantic error]: Method '%s' does not have a 'return' and is of "
            "non-void type.\n",
            name);
    exit(EXIT_FAILURE);
  }
}

void exit_if_invalid_types_at_assignment(AST *exp, Symbol *id) {
  if (exp->info->t_var != id->t_var) {
    fprintf(stderr,
            "[Semantic error]: the identifier '%s' is of type '%s' "
            "but a value of type '%s' is being assigned.\n",
            id->name, data_types_to_string(id->t_var),
            data_types_to_string(exp->info->t_var));
    exit(EXIT_FAILURE);
  }
}

void exit_if_invalid_amount_of_params(AST *params, Symbol *id, char *name) {
  if (params->child_count != id->num_params) {
    fprintf(stderr,
            "[Semantic error]: The method '%s' expects %d parameter(s), "
            "but %d were received.\n",
            name, id->num_params, params->child_count);
    exit(EXIT_FAILURE);
  }
}

void exit_if_missmatch_types_params_at_invocation(AST *params, Symbol *id,
                                                  char *name, int i) {
  if (id->parameter_types[i] != params->children[i]->info->t_var) {
    fprintf(stderr,
            "[Semantic error]: In the call to '%s': "
            "parameter #%d should be of type '%s', "
            "but '%s' was found.\n",
            name, i + 1,
            data_types_to_string(
                id->parameter_types[i]), // converts enum Type to string
            data_types_to_string(params->children[i]->info->t_var));
    exit(EXIT_FAILURE);
  }
}

void exit_if_invalid_predicate_type(AST *condition) {
  if (condition->info->t_var != T_BOOL) {
    fprintf(stderr,
            "[Semantic error]: The condition must be of type "
            "'boolean', but '%s' was found.\n",
            data_types_to_string(condition->info->t_var));
    exit(EXIT_FAILURE);
  }
}

void exit_if_unary_arithmetic_operator_mismatch_types(AST *exp, char *op) {
  if (exp->info->t_var != T_INT) {
    fprintf(stderr,
            "[Semantic error]: the operator '%s' expects an integer "
            "but '%s' was found.\n",
            op, data_types_to_string(exp->info->t_var));
    exit(EXIT_FAILURE);
  }
}

void exit_if_binary_arithmetic_operator_mismatch_types(AST *first_operand,
                                                       AST *second_operand,
                                                       char *op) {
  if (first_operand->info->t_var != T_INT ||
      second_operand->info->t_var != T_INT) {
    fprintf(stderr, "[Semantic error]: the operator '%s' expects two integers",
            op);
    exit(EXIT_FAILURE);
  }
}

void exit_if_unary_boolean_operator_mismatch_types(AST *exp, char *op) {
  if (exp->info->t_var != T_BOOL) {
    fprintf(stderr,
            "[Semantic error]: the operator '%s' expects a boolean expression "
            "but '%s' was found.\n",
            op, data_types_to_string(exp->info->t_var));
    exit(EXIT_FAILURE);
  }
}

void exit_if_binary_boolean_operator_mismatch_types(AST *first_operand,
                                                    AST *second_operand,
                                                    char *op) {
  if (first_operand->info->t_var != T_BOOL ||
      second_operand->info->t_var != T_BOOL) {
    fprintf(stderr, "[Semantic error]: the operator '%s' expects booleans", op);
    exit(EXIT_FAILURE);
  }
}

void exit_if_operators_mismatch_types(AST *first_operand, AST *second_operand,
                                      char *op) {
  if (first_operand->info->t_var != second_operand->info->t_var) {
    fprintf(
        stderr,
        "[Semantic error]: the operator '%s' expects operands of the same type",
        op);
    exit(EXIT_FAILURE);
  }
}

void exit_if_return_with_no_expression(AST *sentence, char *name, int i) {
  if (sentence->child_count <= 0 || sentence->children == NULL ||
      sentence->children[0] == NULL) {
    fprintf(stderr,
            "[Semantic error]: In method '%s': 'return' #%d has no "
            "associated expression.\n",
            name, i + 1);
    exit(EXIT_FAILURE);
  }
}
