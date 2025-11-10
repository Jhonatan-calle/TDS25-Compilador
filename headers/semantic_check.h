#include "ast.h"
#include "types.h"

void exit_if_already_declared(char *name);
void exit_if_not_declared(char *name);
void exit_if_types_invalid_at_declaration(AST *exp, Types type_identifier,
                                          char *name);
void exit_if_invalid_return_type(AST *sentence, int type_identifier, char *name,
                                 int i);
void warning_if_unreachable_code(int i, int sentences_count, char *name);
void exit_if_no_return_in_non_void_method(int return_found, char *name);
void exit_if_invalid_types_at_assignment(AST *exp, Symbol *id);
void exit_if_assigning_a_function(Symbol *id);
void exit_if_invoking_a_variable(AST *exp);
void exit_if_not_invoking_a_function(AST *exp);
void exit_if_invalid_amount_of_params(AST *params, Symbol *id, char *name);
void exit_if_missmatch_types_params_at_invocation(AST *params, Symbol *id,
                                                  char *name, int i);
void exit_if_invalid_predicate_type(AST *condition);
void exit_if_unary_arithmetic_operator_mismatch_types(AST *exp, char *op);
void exit_if_binary_arithmetic_operator_mismatch_types(AST *first_operand,
                                                       AST *second_operand,
                                                       char *op);
void exit_if_unary_boolean_operator_mismatch_types(AST *exp, char *op);
void exit_if_binary_boolean_operator_mismatch_types(AST *first_operand,
                                                    AST *second_operand,
                                                    char *op);
void exit_if_operators_mismatch_types(AST *first_operand, AST *second_operand,
                                      char *op);
void exit_if_return_with_no_expression(AST *sentence, char *name, int i);
