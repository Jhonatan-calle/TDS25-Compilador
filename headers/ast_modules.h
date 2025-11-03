#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "forward_declarations.h"
#include "semantic_check.h"

void set_info_value_depending_operator(AST *node, AST *first_operand,
                                       AST *second_operand, char *op);
void allocate_binary_boolean_node(AST *node, AST *first_operand, AST *second_operand,
                                  char *op);
void allocate_binary_integer_node(AST *node, AST *first_operand, AST *second_operand,
                                  char *op);

void module_switch_case_program(AST *node, va_list args);
void module_switch_case_var_declaration(AST *node, va_list args);
void module_switch_case_method_declaration(AST *node, va_list args);
void module_switch_case_param(AST *node, va_list args);
void module_switch_case_param_list(AST *node, va_list args);
void module_switch_case_block(AST *node, va_list args);
void module_switch_case_assign(AST *node, va_list args);
void module_switch_case_invocation(AST *node, va_list args);
void module_switch_case_if(AST *node, va_list args);
void module_switch_case_else_body(AST *node, va_list args);
void module_switch_case_while(AST *node, va_list args);
void module_switch_case_return(AST *node, va_list args);
void module_switch_case_id(AST *node, va_list args);
void module_switch_case_logic_negation(AST *node, va_list args);
void module_switch_case_arithmetic_negation(AST *node, va_list args);
void module_switch_case_addition(AST *node, va_list args);
void module_switch_case_substraction(AST *node, va_list args);
void module_switch_case_multiplication(AST *node, va_list args);
void module_switch_case_divition(AST *node, va_list args);
void module_switch_case_modulo(AST *node, va_list args);
void module_switch_case_less_than(AST *node, va_list args);
void module_switch_case_greater_than(AST *node, va_list args);
void module_switch_case_equal(AST *node, va_list args);
void module_switch_case_and(AST *node, va_list args);
void module_switch_case_or(AST *node, va_list args);
void module_switch_case_literal(AST *node, va_list args);
void module_switch_case_arg_list(AST *node, va_list args);
