#include "../headers/ast.h"

// Output AST filename
char *ast_filename = "last_generated_ast.sint";
// Local file handle to write the AST
static FILE *ast_out = NULL;

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

AST *new_node(NodeType type, int child_count, ...) {
  AST *node = init_node(type, child_count);

  va_list args;
  va_start(args, child_count);

  switch (type) {
  // Program nodes
  case TR_PROGRAM:
    module_switch_case_program(node, args);
    break;

  // Variable declarations
  case TR_VAR_DECLARATION:
    module_switch_case_var_declaration(node, args);
    break;

  // Method declarations
  case TR_METHOD_DECLARATION:
    module_switch_case_method_declaration(node, args);
    break;

  // Parameters
  case TR_PARAM:
    module_switch_case_param(node, args);
    break;

  case TR_PARAM_LIST:
    module_switch_case_param_list(node, args);
    break;

  // Code blocks
  case TR_BLOCK:
    module_switch_case_block(node, args);
    break;

  // Assignment and invocation
  case TR_ASSIGN:
    module_switch_case_assign(node, args);
    break;

  case TR_INVOCATION:
    module_switch_case_invocation(node, args);
    break;

  // Flow control
  case TR_IF_STATEMENT:
    module_switch_case_if(node, args);
    break;

  case TR_ELSE_BODY:
    module_switch_case_else_body(node, args);
    break;

  case TR_WHILE_STATEMENT:
    module_switch_case_while(node, args);
    break;

  case TR_RETURN:
    module_switch_case_return(node, args);
    break;

  // Operators and expressions
  case TR_LOGIC_NEGATION:
    module_switch_case_logic_negation(node, args);
    break;

  case TR_ARITHMETIC_NEGATION:
    module_switch_case_arithmetic_negation(node, args);
    break;

  case TR_ADDITION:
    module_switch_case_addition(node, args);
    break;

  case TR_SUBSTRACTION:
    module_switch_case_substraction(node, args);
    break;

  case TR_MULTIPLICATION:
    module_switch_case_multiplication(node, args);
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
    // char *str = strcat("Pass Case: ", node_type_to_string(type));
    // print_if_debug_flag(str);
    break;
  default:
    fprintf(stderr, "Warning: Node type not handled in new_node: %s\n",
            node_type_to_string(type));
    node->children = NULL;
    break;
  }

  va_end(args);

  if (debug_flag) {
    printf("[DEBUG NEW_NODE] Node %s finalized\t\t name=%s\t child_count=%d\n",
           node_type_to_string(type), (node->info) ? node->info->name : "_",
           node->child_count);
  }

  return node;
}

AST *append_child(AST *list, AST *child) {
  list->children =
      realloc(list->children, sizeof(AST *) * (list->child_count + 1));
  if (!list->children) {
    fprintf(stderr, "Error realloc in append_child\n");
    exit(EXIT_FAILURE);
  }
  list->children[list->child_count] = child;
  list->child_count += 1;

  return list;
}

void free_ast(AST *node) {
  if (!node)
    return;

  for (int i = 0; i < node->child_count; i++)
    free_ast(node->children[i]);
  free(node->children);
  free(node);
}

void save_ast_in_file(AST *root) {
  ast_out = fopen(ast_filename, "w");
  if (!ast_out) {
    perror("Error opening AST output file");
  }
  fprintf(ast_out, "\n===== ABSTRACT SYNTAX TREE =====\n");
  write_ast_in_file(root, 0);
  fprintf(ast_out, "\n================================\n");
  // Close the output file if we opened one
  if (ast_out) {
    fclose(ast_out);
    ast_out = NULL;
    printf("Abstract Syntax Tree generated at file: %s\n", ast_filename);
    print_generated_ast_if_debug_flag();
  }
}

/**
 * AST utility function
 * Prints to the console the generated & saved AST if the debug flag is set
 */
void print_generated_ast_if_debug_flag() {
  if (debug_flag) {
    printf("[DEBUG] Generated AST\n");
    FILE *f;
    int c;
    if ((f = fopen(ast_filename, "r")) == NULL) {
      printf("error in opening a file");
      exit(1);
    }

    while ((c = fgetc(f)) != EOF) {
      printf("%c", c); // printing to the console
    }

    fclose(f);
  }
}

/**
 * Write the AST into the global var filename
 */
void write_ast_in_file(AST *node, int depth) {
  if (node == NULL)
    return;

  // Visual indentation
  for (int i = 0; i < depth; i++) {
    fprintf(ast_out, "  ");
  }

  // Print node type
  fprintf(ast_out, "%s", node_type_to_string(node->type));

  if (node->child_count <= 0) {
    fprintf(ast_out, "\n");
    return;
  }

  if (node < 0) {
    fprintf(ast_out, "Invalid Node!\n");
    return;
  }

  // If it has semantic information, print it
  if (node->info != NULL) {
    fprintf(ast_out, " [t_var=%s", data_types_to_string(node->info->t_var));
    if (node->info->name != NULL)
      fprintf(ast_out, ", name=%s", node->info->name);
    fprintf(ast_out, ", value=%d]", node->info->value);
  }

  fprintf(ast_out, "\n");

  // Recursion in the children
  for (int i = 0; i < node->child_count; i++) {
    write_ast_in_file(node->children[i], depth + 1);
  }
}
