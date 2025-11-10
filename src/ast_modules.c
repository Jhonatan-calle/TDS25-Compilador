#include "../headers/ast_modules.h"

void allocate_binary_boolean_node(AST *node, AST *first_operand,
                                  AST *second_operand, char *op) {
  node->info = malloc(sizeof(Symbol));
  node->info->t_var = T_BOOL;
  set_info_value_depending_operator(node, first_operand, second_operand, op);
  node->child_count = 2;
  node->children = malloc(sizeof(AST *) * 2);
  node->children[0] = first_operand;
  node->children[1] = second_operand;
}

void allocate_binary_integer_node(AST *node, AST *first_operand,
                                  AST *second_operand, char *op) {
  node->info = malloc(sizeof(Symbol));
  node->info->t_var = T_INT;
  set_info_value_depending_operator(node, first_operand, second_operand, op);
  node->child_count = 2;
  node->children = malloc(sizeof(AST *) * 2);
  node->children[0] = first_operand;
  node->children[1] = second_operand;
}

AST *init_node(NodeType type, int child_count) {
  AST *node = malloc(sizeof(AST));
  if (!node) {
    fprintf(stderr, "<<<<<Error: could not allocate memory for AST>>>>>\n");
    exit(EXIT_FAILURE);
  }

  node->type = type;
  node->info = NULL;
  node->child_count = child_count;
  if (child_count > 0) {
    node->children = malloc(sizeof(AST *) * child_count);
    if (!node->children) {
      perror("malloc");
      exit(1);
    }
    for (int i = 0; i < child_count; i++)
      node->children[i] = NULL; // initialize pointers
  } else {
    node->children = NULL;
  }
  return node;
}

int checks_returns(AST *root, char *name, int type_identifier) {
  if (!root)
    return 0;
  int sentences_count = root->child_count;
  for (int i = 0; i < sentences_count; i++) {
    AST *sentence = root->children[i];
    if (!sentence)
      continue;

    switch (sentence->type) {

    case TR_RETURN:
      exit_if_return_with_no_expression(sentence, name, i);
      exit_if_invalid_return_type(sentence, type_identifier, name, i);
      warning_if_unreachable_code(i, sentences_count, name);
      return 1;

    case TR_IF_STATEMENT:
      // To make sure that the expected childrens exists
      if (sentence->child_count < 3)
        return 0;
      AST *if_stmt_list = sentence->children[2];
      if (!if_stmt_list)
        return 0;
      int if_count = if_stmt_list->child_count;

      // total count: if guard + whatever comes after in the block
      int tail_count = sentences_count - (i + 1);
      int branch_if_count = if_count + tail_count;
      AST *branch_if = init_node(TR_AUXILIAR_NODE, branch_if_count);
      // Reserving memory for the array of children
      branch_if->children = malloc(sizeof(AST *) * branch_if_count);
      if (!branch_if->children) {
        perror("malloc");
        exit(EXIT_FAILURE);
      }

      // Copy body of the if at the beginning
      for (int k = 0; k < if_count; k++)
        branch_if->children[k] = if_stmt_list->children[k];

      // Copy the rest of sentences after the if (tail)
      for (int k = 0; k < tail_count; k++)
        branch_if->children[if_count + k] = root->children[i + 1 + k];

      // If there is an else statement
      if (sentence->child_count > 3 && sentence->children[3]) {
        AST *else_block = sentence->children[3];
        if (else_block->child_count < 2)
          return checks_returns(branch_if, name, type_identifier);
        AST *else_stmt_list = else_block->children[1];
        if (!else_stmt_list)
          return checks_returns(branch_if, name, type_identifier);
        int else_count = else_stmt_list->child_count;
        int branch_else_count = else_count + tail_count;
        AST *branch_else = init_node(TR_AUXILIAR_NODE, branch_else_count);
        branch_else->children = malloc(sizeof(AST *) * branch_else_count);
        if (!branch_else->children) {
          perror("malloc");
          exit(EXIT_FAILURE);
        }

        for (int k = 0; k < else_count; k++)
          branch_else->children[k] = else_stmt_list->children[k];
        for (int k = 0; k < tail_count; k++)
          branch_else->children[else_count + k] = root->children[i + 1 + k];

        return checks_returns(branch_if, name, type_identifier) &&
               checks_returns(branch_else, name, type_identifier);
      }

      return checks_returns(branch_if, name, type_identifier);

    default:
      break;
    }
  }
  return 0;
}

void set_info_value_depending_operator(AST *node, AST *first_operand,
                                       AST *second_operand, char *op) {
  if (strcmp(op, "==") == 0) {
    node->info->value =
        first_operand->info->value == second_operand->info->value;
  } else if (strcmp(op, "&&") == 0) {
    node->info->value =
        first_operand->info->value && second_operand->info->value;
  } else if (strcmp(op, "||") == 0) {
    node->info->value =
        first_operand->info->value || second_operand->info->value;
  } else if (strcmp(op, "<") == 0) {
    node->info->value =
        first_operand->info->value < second_operand->info->value;
  } else if (strcmp(op, "<=") == 0) {
    node->info->value =
        first_operand->info->value <= second_operand->info->value;
  } else if (strcmp(op, ">") == 0) {
    node->info->value =
        first_operand->info->value > second_operand->info->value;
  } else if (strcmp(op, ">=") == 0) {
    node->info->value =
        first_operand->info->value >= second_operand->info->value;
  } else if (strcmp(op, "+") == 0) {
    node->info->value =
        first_operand->info->value + second_operand->info->value;
  } else if (strcmp(op, "-") == 0) {
    node->info->value =
        first_operand->info->value - second_operand->info->value;
  } else if (strcmp(op, "*") == 0) {
    node->info->value =
        first_operand->info->value * second_operand->info->value;
  } else if (strcmp(op, "/") == 0) {
    node->info->value =
        first_operand->info->value / second_operand->info->value;
  } else if (strcmp(op, "%") == 0) {
    node->info->value =
        first_operand->info->value % second_operand->info->value;
  } else {
    exit(EXIT_FAILURE);
  }
}

void module_switch_case_program(AST *node, va_list args) {
  node->child_count = 1;
  node->children = malloc(sizeof(AST *));
  node->children[0] = va_arg(args, AST *); // $1: declaration_list, of type AST*
}

void module_switch_case_var_declaration(AST *node, va_list args) {
  int type_identifier =
      va_arg(args, int); // $1: types, the enum of types (internally a int)
  char *name = va_arg(args, char *); // $2: ID, the declared var name
  exit_if_already_declared(name);

  AST *exp = va_arg(args, AST *);

  exit_if_invoking_a_variable(exp);
  exit_if_not_invoking_a_function(exp);
  exit_if_types_invalid_at_declaration(exp, type_identifier, name);

  Symbol *symbol = malloc(sizeof(Symbol));
  symbol->t_var = type_identifier; // type (enum Types)
  symbol->name = name;             // identifier
  symbol->category = S_VAR;
  symbol->value = exp->info->value;
  insert_symbol(symbol);
  node->info = symbol;
  node->child_count = 1;
  node->children = malloc(sizeof(AST *));
  node->children[0] = exp;
}

void module_switch_case_method_declaration(AST *node, va_list args) {
  int type_identifier = va_arg(args, int);
  char *name = va_arg(args, char *); // $2: ID, the declared var name
  exit_if_already_declared(name);

  Symbol *symbol = malloc(sizeof(Symbol));
  symbol->t_var = type_identifier; // type (enum Types)
  symbol->name = name;             // identifier
  symbol->category = S_FUNC;
  AST *params = va_arg(args, AST *);
  if (params) {
    symbol->num_params = params->child_count;
    symbol->parameter_types = malloc(sizeof(Types) * symbol->num_params);

    for (int i = 0; i < symbol->num_params; i++) {
      symbol->parameter_types[i] = params->children[i]->info->t_var;
      insert_symbol(params->children[i]->info);
    }
  }
  AST *body = va_arg(args, AST *);
  // TR_METHOD_DECLARATION that does not have Extern reserved keyword
  if (body->type == TR_BLOCK && type_identifier != T_VOID) {

    int return_found = 0;

    return_found = checks_returns(body->children[1], name, type_identifier);

    exit_if_no_return_in_non_void_method(return_found, name);

    symbol->body = body;
  }
  insert_symbol(symbol);
  node->info = symbol;
  node->child_count = 2;
  node->children = malloc(sizeof(AST *) * 2);
  node->children[0] = params;
  node->children[1] = body;
}

void module_switch_case_param(AST *node, va_list args) {
  int type_identifier = va_arg(args, int);
  char *name = va_arg(args, char *); // $2: ID, the declared var name
  exit_if_already_declared(name);

  Symbol *symbol = malloc(sizeof(Symbol));
  symbol->t_var = type_identifier;
  symbol->name = name;
  symbol->category = S_VAR;
  symbol->value = 0; // init value of 0 for params
  insert_symbol(symbol);
  node->info = symbol;
}

void module_switch_case_param_list(AST *node, va_list args) {
  AST *param = va_arg(args, AST *);
  node->child_count = 1;
  node->children = malloc(sizeof(AST *));
  node->children[0] = param;
}

void module_switch_case_block(AST *node, va_list args) {
  node->child_count = 2;
  node->children = malloc(sizeof(AST *) * 2);
  // declaration list
  node->children[0] = va_arg(args, AST *);
  // $3: statement_list, of type AST*
  node->children[1] = va_arg(args, AST *);
}

void module_switch_case_assign(AST *node, va_list args) {
  char *name = va_arg(args, char *); // $1: ID, the var name to assign
  exit_if_not_declared(name);

  // Try to assign locally, if it doesn't found it, search globally
  Symbol *id = search_symbol_locally(name);
  if (!id)
    id = search_symbol_globally(name);

  node->info = id;

  AST *exp = va_arg(args, AST *);

  exit_if_assigning_a_function(id);
  exit_if_invoking_a_variable(exp);
  exit_if_not_invoking_a_function(exp);
  exit_if_invalid_types_at_assignment(exp, id);

  id->value = exp->info->value;
  node->child_count = 2;
  node->children = malloc(sizeof(AST *) * 2);

  AST *id_ast = malloc(sizeof(AST));
  id_ast->type = TR_IDENTIFIER;
  id_ast->info = id;
  id_ast->child_count = 0;
  id_ast->children = NULL;

  node->children[0] = id_ast;
  node->children[1] = exp;
}

void module_switch_case_invocation(AST *node, va_list args) {
  char *name = va_arg(args, char *); // $1: ID, the var name to assign

  exit_if_not_declared(name);

  // Try to call locally, if it doesn't found it, search globally
  Symbol *id = search_symbol_locally(name);
  if (!id)
    id = search_symbol_globally(name);

  AST *args_invocation = va_arg(args, AST *);
  if (args_invocation != NULL) {
    exit_if_invalid_amount_of_params(args_invocation, id, name);

    for (int i = 0; i < id->num_params; i++) {
      exit_if_missmatch_types_params_at_invocation(args_invocation, id, name,
                                                   i);
    }
    node->child_count = 1;
    node->children = malloc(sizeof(AST *));
    node->children[0] = args_invocation;
  } else {
    node->child_count = 0;
    node->children = NULL;
  }
  Symbol *info_invocation = malloc(sizeof(Symbol));
  info_invocation->category = id->category;
  info_invocation->body = id->body;
  info_invocation->name = id->name;
  info_invocation->num_params = id->num_params;
  info_invocation->parameter_types = id->parameter_types;
  info_invocation->t_var = id->t_var;
  node->info = info_invocation;
}

void module_switch_case_if(AST *node, va_list args) {
  AST *condition = va_arg(args, AST *);
  exit_if_invalid_predicate_type(condition);

  AST *body_declarations = va_arg(args, AST *);
  AST *body_statements = va_arg(args, AST *);
  AST *else_body = va_arg(args, AST *);
  node->child_count = 4;
  node->children = malloc(sizeof(AST *) * 4);
  node->children[0] = condition;
  node->children[1] = body_declarations;
  node->children[2] = body_statements;
  node->children[3] = else_body;
}

void module_switch_case_else_body(AST *node, va_list args) {
  AST *declaration_list = va_arg(args, AST *);
  AST *statement_list = va_arg(args, AST *);

  node->child_count = 2;
  node->children = malloc(sizeof(AST *) * 2);
  node->children[0] = declaration_list;
  node->children[1] = statement_list;
}

void module_switch_case_while(AST *node, va_list args) {
  AST *condition = va_arg(args, AST *);
  exit_if_invalid_predicate_type(condition);

  AST *body = va_arg(args, AST *);

  node->child_count = 2;
  node->children = malloc(sizeof(AST *) * 2);
  node->children[0] = condition;
  node->children[1] = body;
}

void module_switch_case_return(AST *node, va_list args) {
  if (node->child_count == 1) {
    AST *maybe_expr = va_arg(args, AST *);
    exit_if_invoking_a_variable(maybe_expr);
    exit_if_not_invoking_a_function(maybe_expr);
    node->children = malloc(sizeof(AST *));
    node->children[0] = maybe_expr;
  } else {
    node->child_count = 0;
    node->children = NULL;
  }
}

void module_switch_case_id(AST *node, va_list args) {
  char *name = va_arg(args, char *); // $1: ID, the var name
  exit_if_not_declared(name);

  // Try to search locally, if it doesn't found it, search globally
  Symbol *id = search_symbol_locally(name);
  if (!id)
    id = search_symbol_globally(name);

  node->info = id;
  node->child_count = 0;
  node->children = NULL;
}

void module_switch_case_arithmetic_negation(AST *node, va_list args) {
  AST *exp = va_arg(args, AST *);
  exit_if_unary_arithmetic_operator_mismatch_types(exp, "-");

  node->info = malloc(sizeof(Symbol));
  node->info->t_var = T_INT;
  node->info->value = -(exp->info->value);
  node->child_count = 1;
  node->children = malloc(sizeof(AST *));
  node->children[0] = exp;
}

void module_switch_case_addition(AST *node, va_list args) {
  AST *first_operand = va_arg(args, AST *);
  AST *second_operand = va_arg(args, AST *);
  char *op = "+";
  exit_if_binary_arithmetic_operator_mismatch_types(first_operand,
                                                    second_operand, op);

  allocate_binary_integer_node(node, first_operand, second_operand, op);
}

void module_switch_case_substraction(AST *node, va_list args) {
  AST *first_operand = va_arg(args, AST *);
  AST *second_operand = va_arg(args, AST *);
  char *op = "-";
  exit_if_binary_arithmetic_operator_mismatch_types(first_operand,
                                                    second_operand, op);

  allocate_binary_integer_node(node, first_operand, second_operand, op);
}

void module_switch_case_multiplication(AST *node, va_list args) {
  AST *first_operand = va_arg(args, AST *);
  AST *second_operand = va_arg(args, AST *);
  char *op = "*";
  exit_if_binary_arithmetic_operator_mismatch_types(first_operand,
                                                    second_operand, op);

  allocate_binary_integer_node(node, first_operand, second_operand, op);
}

void module_switch_case_divition(AST *node, va_list args) {
  AST *first_operand = va_arg(args, AST *);
  AST *second_operand = va_arg(args, AST *);
  char *op = "/";
  exit_if_binary_arithmetic_operator_mismatch_types(first_operand,
                                                    second_operand, op);

  allocate_binary_integer_node(node, first_operand, second_operand, op);
}

void module_switch_case_modulo(AST *node, va_list args) {
  AST *first_operand = va_arg(args, AST *);
  AST *second_operand = va_arg(args, AST *);
  char *op = "%";
  exit_if_binary_arithmetic_operator_mismatch_types(first_operand,
                                                    second_operand, op);

  allocate_binary_integer_node(node, first_operand, second_operand, op);
}

void module_switch_case_logic_negation(AST *node, va_list args) {
  AST *exp = va_arg(args, AST *);
  exit_if_unary_boolean_operator_mismatch_types(exp, "!");

  node->info = malloc(sizeof(Symbol));
  node->info->t_var = T_BOOL;
  node->info->value = !exp->info->value;
  node->info->category = exp->info->category;
  node->child_count = 1;
  node->children = malloc(sizeof(AST *));
  node->children[0] = exp;
}

void module_switch_case_less_than(AST *node, va_list args) {
  AST *first_operand = va_arg(args, AST *);
  AST *second_operand = va_arg(args, AST *);
  char *op = "<";
  exit_if_binary_arithmetic_operator_mismatch_types(first_operand,
                                                    second_operand, op);

  allocate_binary_boolean_node(node, first_operand, second_operand, op);
}

void module_switch_case_less_eq_than(AST *node, va_list args) {
  AST *first_operand = va_arg(args, AST *);
  AST *second_operand = va_arg(args, AST *);
  char *op = "<=";
  exit_if_binary_arithmetic_operator_mismatch_types(first_operand,
                                                    second_operand, op);

  allocate_binary_boolean_node(node, first_operand, second_operand, op);
}

void module_switch_case_greater_than(AST *node, va_list args) {
  AST *first_operand = va_arg(args, AST *);
  AST *second_operand = va_arg(args, AST *);
  char *op = ">";
  exit_if_binary_arithmetic_operator_mismatch_types(first_operand,
                                                    second_operand, op);

  allocate_binary_boolean_node(node, first_operand, second_operand, op);
}

void module_switch_case_greater_eq_than(AST *node, va_list args) {
  AST *first_operand = va_arg(args, AST *);
  AST *second_operand = va_arg(args, AST *);
  char *op = ">=";
  exit_if_binary_arithmetic_operator_mismatch_types(first_operand,
                                                    second_operand, op);

  allocate_binary_boolean_node(node, first_operand, second_operand, op);
}

void module_switch_case_equal(AST *node, va_list args) {
  AST *first_operand = va_arg(args, AST *);
  AST *second_operand = va_arg(args, AST *);
  char *op = "==";
  exit_if_operators_mismatch_types(first_operand, second_operand, op);

  allocate_binary_boolean_node(node, first_operand, second_operand, op);
}

void module_switch_case_and(AST *node, va_list args) {
  AST *first_operand = va_arg(args, AST *);
  AST *second_operand = va_arg(args, AST *);
  char *op = "&&";
  exit_if_binary_boolean_operator_mismatch_types(first_operand, second_operand,
                                                 op);

  allocate_binary_boolean_node(node, first_operand, second_operand, op);
}

void module_switch_case_or(AST *node, va_list args) {
  AST *first_operand = va_arg(args, AST *);
  AST *second_operand = va_arg(args, AST *);
  char *op = "||";
  exit_if_binary_boolean_operator_mismatch_types(first_operand, second_operand,
                                                 op);

  allocate_binary_boolean_node(node, first_operand, second_operand, op);
}

void module_switch_case_literal(AST *node, va_list args) {
  node->info = malloc(sizeof(Symbol));
  node->info->t_var =
      va_arg(args, int); // T_INT or T_BOOL, represented internally as a int
  node->info->name = "TR_VALUE";
  node->info->value = va_arg(
      args, int); // $1 if its a numeric value, 0 if its false or 1 if its true
  node->child_count = 0;
}

void module_switch_case_arg_list(AST *node, va_list args) {
  node->child_count = 1;
  node->children = malloc(sizeof(AST *) * 2);
  node->children[0] = va_arg(args, AST *);
}
