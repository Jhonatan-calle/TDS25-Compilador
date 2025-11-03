#include "../headers/assembly_code.h"

/* --- Variables --- */
// Output assembly filename
char *assembly_filename = "last_generated_assembly.ass";
// Local file handle to write assembly
static FILE *asm_out = NULL;
// Counter of temporals
int temp_counter = 0;
// Number of variables/params (slots)
int local_slots = 0;
// Number of temps (slots)
int temp_slots = 0;
// Name of the first six registers
static const char *arg_registers[] = {"%rdi", "%rsi", "%rdx",
                                      "%rcx", "%r8",  "%r9"};
// Index of the next free register
static int next_arg = 0;

/* --- Functions --- */
/**
 * Resets the global local and temp slots to zero
 */
void enter_function() {
  local_slots = 0;
  temp_slots = 0;
}

/**
 * Allocate a new local storage entry for the current function/frame.
 * Return the new amount of local slots
 */
int alloc_local() {
  local_slots += 1;          // A new slot of 8 bytes
  return -(local_slots * 8); // -8, -16, -24, ...
}

/**
 * Allocs a temp *after* the locals. Returns a unique negative offset
 *    Stores the temp after the locals:
 *    offset = -((local_slots + temp_index) * 8)
 */
int alloc_temp() {
  temp_slots += 1;
  return -((local_slots + temp_slots) * 8);
}

/**
 * Compute and assign memory offsets for AST declarations
 *
 * Go through the AST and compute the offsets for declarations,
 * parameters and other entities that need storage.
 */
void gen_offsets(AST *root) {
  if (!root)
    return;

  switch (root->type) {

  case TR_PROGRAM:
    // Iterate children
    for (int i = 0; i < root->child_count; i++)
      gen_offsets(root->children[i]);
    break;

  case TR_METHOD_DECLARATION:
    enter_function();

    // Process parameters first
    for (int i = 0; root->children[0] && i < root->children[0]->child_count;
         i++) {
      AST *param = root->children[0]->children[i];
      param->info->offset = alloc_local();
    }

    // Now process the body of the function
    gen_offsets(root->children[1]);

    root->info->offset = (local_slots + temp_slots); // final size of the frame
    break;

  case TR_VAR_DECLARATION:
    root->info->offset = alloc_local();
    gen_offsets(root->children[0]); // initializer
    break;

  case TR_ASSIGN:
    gen_offsets(root->children[1]);
    root->info->offset = root->children[0]->info->offset;
    break;

  case TR_VALUE:
    root->info->offset = 0;
    break;

  case TR_INVOCATION:
    // Reserve temps for each argument
    if (root->child_count > 0)
      gen_offsets(root->children[0]);
    root->info->offset = alloc_temp();
    break;

  case TR_ARG_LIST:
    for (int i = 0; i < root->child_count; i++)
      gen_offsets(root->children[i]);
    break;

  case TR_ADDITION:
  case TR_SUBSTRACTION:
  case TR_MULTIPLICATION:
  case TR_DIVITION:
  case TR_MODULO:
  case TR_LESS_THAN:
  case TR_LESS_EQ_THAN:
  case TR_GREATER_THAN:
  case TR_GREATER_EQ_THAN:
  case TR_LOGIC_EQUAL:
  case TR_AND:
  case TR_OR:
  case TR_LOGIC_NEGATION:
  case TR_ARITHMETIC_NEGATION:
    // Children first
    gen_offsets(root->children[0]);
    if (root->child_count > 1)
      gen_offsets(root->children[1]);
    root->info->offset = alloc_temp();
    break;

  case TR_IF_STATEMENT:
  case TR_WHILE_STATEMENT:
  case TR_RETURN:
  case TR_BLOCK:
  case TR_SENTENCES_LIST:
  case TR_ELSE_BODY:
    for (int i = 0; i < root->child_count; i++)
      gen_offsets(root->children[i]);
    break;

  default:
    // Fallback safe
    for (int i = 0; i < root->child_count; i++)
      gen_offsets(root->children[i]);
    break;
  }
}

/**
 * Assembly util function
 *
 * Called when a new use of a register is used
 * Uses temp_counter global variable to increment or decrement the amount of
 * registers used
 */
char *new_temp() {
  char buffer[16];
  snprintf(buffer, sizeof(buffer), "t%d", temp_counter++);
  return strdup(buffer);
}

/**
 * Returns the next argument register available
 */
const char *get_arg_register() {
  static char stack_arg[32]; // buffer to return addresses like "16(%rbp)"

  if (next_arg < 6) {
    // First 6 arguments: registers
    return arg_registers[next_arg++];
  } else {
    // From the seventh argument: stack
    int stack_offset = 16 + (next_arg - 6) * 8;
    // 16(%rbp) --> first argument in the stack (seventh total)
    snprintf(stack_arg, sizeof(stack_arg), "%d(%%rbp)", stack_offset);
    next_arg++;
    return stack_arg;
  }
}

/**
 * Resets the use of argument registers (after a call)
 */
void reset_arg_registers() { next_arg = 0; }

/**
 * Assembly util function
 * It generates the assembly code using the generated TAC List
 */
void gen_assembly_code(TAC *head) {
  if (!head)
    return;

  // Open (or switch) output to the file indicated by assembly_filename.
  // If unavailable or failing to open, we fallback to stdout.
  if (assembly_filename && *assembly_filename) {
    asm_out = fopen(assembly_filename, "w");
    if (!asm_out) {
      perror("Error opening assembly output file");
    }
  }

  bool in_function = false;

  fprintf(asm_out, "  .data");
  TAC *q;

  for (q = head; q && !in_function; q = q->next) {
    if (q->op == TAC_ASSIGN) {
      // If the result is global (without a function activated)
      fprintf(asm_out, "%s:\n", q->result->name);
      fprintf(asm_out, "  .quad %d\n", q->op1 ? q->op1->value : 0);
    }

    if (q->op == TAC_LABEL) {
      // When starts a function
      in_function = true;
      break;
    }
  }

  fprintf(asm_out, "\n  .text\n");

  for (TAC *t = q; t; t = t->next) {
    switch (t->op) {

    // --- Arithmetic operations ---
    case TAC_ADD:
      assembly_module_add(asm_out, t);
      break;

    case TAC_SUB:
      assembly_module_sub(asm_out, t);
      break;

    case TAC_MUL:
      assembly_module_mul(asm_out, t);
      break;

    case TAC_DIV:
      assembly_module_div(asm_out, t);
      break;

    case TAC_MOD:
      assembly_module_mod(asm_out, t);
      break;

    // --- Comparations ---
    case TAC_LESS:
      assembly_module_less(asm_out, t);
      break;

    case TAC_LESS_EQ:
      assembly_module_less_eq(asm_out, t);
      break;

    case TAC_GR:
      assembly_module_gr(asm_out, t);
      break;

    case TAC_GREATER_EQ:
      assembly_module_greater_eq(asm_out, t);
      break;

    case TAC_EQ:
      assembly_module_eq(asm_out, t);
      break;

    // --- Logicals ---
    case TAC_AND:
      assembly_module_and(asm_out, t);
      break;

    case TAC_OR:
      assembly_module_or(asm_out, t);
      break;

    case TAC_NOT:
      assembly_module_not(asm_out, t);
      break;

    // --- Unaries ---
    case TAC_NEG:
      assembly_module_neg(asm_out, t);
      break;

    // --- Assign ---
    case TAC_ASSIGN:
      assembly_module_assign(asm_out, t);
      break;

    // --- Flow control ---
    case TAC_LABEL:
      assembly_module_label(asm_out, t);
      break;

    case TAC_LABEL_END:
    case TAC_LABEL_IF:
      assembly_module_label_if(asm_out, t);
      break;

    case TAC_GOTO:
      assembly_module_goto(asm_out, t);
      break;

    case TAC_IFZ:
      assembly_module_ifz(asm_out, t);
      break;

    // --- Functions ---
    case TAC_PARAM:
      assembly_module_param(asm_out, t);
      break;

    case TAC_ARG:
      assembly_module_arg(asm_out, t);
      break;

    case TAC_CALL:
      assembly_module_call(asm_out, t);
      break;

    case TAC_RETURN:
      assembly_module_return(asm_out, t);
      break;

    case TAC_EXTERN:
      assembly_module_extern(asm_out, t);
      break;

    case TAC_UNKNOWN:
    default:
      fprintf(asm_out, "  ; unknown TAC op %d\n", t->op);
      break;
    }
  }
  fprintf(asm_out, "  .section  .note.GNU-stack,\"\",@progbits\n");
  fprintf(asm_out, "\n");

  // Close the output file if we opened one
  if (asm_out) {
    fclose(asm_out);
    asm_out = NULL;
    printf("Assembly generated at file: %s\n", assembly_filename);
    print_generated_assembly_if_debug_flag();
  }
}

/**
 * Assembly utility function
 * Prints to the console the generated & saved assembly if the debug flag is set
 */
void print_generated_assembly_if_debug_flag() {
  if (debug_flag) {
    printf("[DEBUG] Generated Assembly\n");
    FILE *f;
    int c;
    if ((f = fopen(assembly_filename, "r")) == NULL) {
      printf("error in opening a file");
      exit(1);
    }

    while ((c = fgetc(f)) != EOF) {
      printf("%c", c); // printing to the console
    }

    fclose(f);
  }
}
