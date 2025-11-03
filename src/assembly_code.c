#include "../headers/assembly_code.h"

/* --- Variables --- */
// Output assembly filename
char *assembly_filename = "last_generated_assembly.ass";
// Local file handle to write assembly; fallback to stdout if not set
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
  case TR_GREATER_THAN:
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
    case TAC_ADD: {
      reset_arg_registers();
      // We use %r10 as a temporal register (scratch)
      // We support immediate operands (offset == 0) and memory
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      if (t->op2->offset == 0)
        fprintf(asm_out, "  add $%d, %%r10\n", t->op2->value);
      else
        fprintf(asm_out, "  add %d(%%rbp), %%r10\n", t->op2->offset);

      fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_SUB: {
      reset_arg_registers();
      // result = op1 - op2
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      if (t->op2->offset == 0)
        fprintf(asm_out, "  sub $%d, %%r10\n", t->op2->value);
      else
        fprintf(asm_out, "  sub %d(%%rbp), %%r10\n", t->op2->offset);

      fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_MUL: {
      reset_arg_registers();
      // imul supports reg, r/m form: imul <r/m64>, <reg>
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      if (t->op2->offset == 0)
        fprintf(asm_out, "  imul $%d, %%r10\n",
                t->op2->value); // immediate multiply
      else
        fprintf(asm_out, "  imul %d(%%rbp), %%r10\n", t->op2->offset);

      fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_DIV: {
      reset_arg_registers();
      // idiv requires dividend in rax, sign-extend in rdx via cqo
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%rax\n", t->op1->value);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%rax\n", t->op1->offset);

      fprintf(asm_out, "  cqo\n");

      if (t->op2->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n  idiv %%r10\n", t->op2->value);
      else
        fprintf(asm_out, "  idiv %d(%%rbp)\n", t->op2->offset);

      fprintf(asm_out, "  mov %%rax, %d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_MOD: {
      reset_arg_registers();
      // remainder stored in rdx after idiv
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%rax\n", t->op1->value);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%rax\n", t->op1->offset);

      fprintf(asm_out, "  cqo\n");

      if (t->op2->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n  idiv %%r10\n", t->op2->value);
      else
        fprintf(asm_out, "  idiv %d(%%rbp)\n", t->op2->offset);

      fprintf(asm_out, "  mov %%rdx, %d(%%rbp)\n", t->result->offset);
      break;
    }

    // --- Comparations ---
    case TAC_LESS: {
      reset_arg_registers();
      // result = op1 < op2
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      if (t->op2->offset == 0)
        fprintf(asm_out, "  cmp $%d, %%r10\n", t->op2->value);
      else
        fprintf(asm_out, "  cmp %d(%%rbp), %%r10\n", t->op2->offset);

      fprintf(asm_out, "  setl %%al\n");
      fprintf(asm_out, "  movzbq %%al, %%r10\n");
      fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_GR: {
      reset_arg_registers();
      // result = op1 > op2
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      if (t->op2->offset == 0)
        fprintf(asm_out, "  cmp $%d, %%r10\n", t->op2->value);
      else
        fprintf(asm_out, "  cmp %d(%%rbp), %%r10\n", t->op2->offset);

      fprintf(asm_out, "  setg %%al\n");
      fprintf(asm_out, "  movzbq %%al, %%r10\n");
      fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_EQ: {
      reset_arg_registers();
      // result = op1 == op2
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      if (t->op2->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r11\n", t->op2->value);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r11\n", t->op2->offset);

      fprintf(asm_out, "  cmp %%r10, %%r11\n");
      fprintf(asm_out, "  mov $0, %%r11\n");
      fprintf(asm_out, "  mov $1, %%r10\n");
      fprintf(asm_out, "  cmove %%r10, %%r11\n");
      fprintf(asm_out, "  mov %%r11, %d(%%rbp)\n", t->result->offset);
      break;
    }

    // --- Logicals ---
    case TAC_AND: {
      reset_arg_registers();
      // booleanize op1 and op2, then and
      // use r10 and r11 as scratch
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      fprintf(asm_out, "  cmp $0, %%r10\n");
      fprintf(asm_out, "  setne %%al\n");
      fprintf(asm_out, "  movzbq %%al, %%r10\n");

      if (t->op2->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r11\n", t->op2->value);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r11\n", t->op2->offset);

      fprintf(asm_out, "  cmp $0, %%r11\n");
      fprintf(asm_out, "  setne %%bl\n");
      fprintf(asm_out, "  and %%bl, %%al\n");
      fprintf(asm_out, "  movzbq %%al, %%r10\n");
      fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_OR: {
      reset_arg_registers();
      // booleanize op1 and op2, then or
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      fprintf(asm_out, "  cmp $0, %%r10\n");
      fprintf(asm_out, "  setne %%al\n");

      if (t->op2->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r11\n", t->op2->value);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r11\n", t->op2->offset);

      fprintf(asm_out, "  cmp $0, %%r11\n");
      fprintf(asm_out, "  setne %%bl\n");
      fprintf(asm_out, "  or %%bl, %%al\n");
      fprintf(asm_out, "  movzbq %%al, %%r10\n");
      fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_NOT: {
      reset_arg_registers();
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      fprintf(asm_out, "  cmp $0, %%r10\n");
      fprintf(asm_out, "  sete %%al\n");
      fprintf(asm_out, "  movzbq %%al, %%r10\n");
      fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
      break;
    }

    // --- Unaries ---
    case TAC_NEG: {
      reset_arg_registers();
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      fprintf(asm_out, "  neg %%r10\n");
      fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
      break;
    }

    // --- Assign ---
    case TAC_ASSIGN: {
      reset_arg_registers();
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
      break;
    }

    // --- Flow control ---
    case TAC_LABEL:
      reset_arg_registers();
      fprintf(asm_out, "  .globl %s\n", t->result->name);
      fprintf(asm_out, "%s:\n", t->result->name);
      // If t->result->offset represents amount to reserve, it keeps.
      fprintf(asm_out, "  enter $(8 * %d), $0\n", t->result->offset);
      break;

    case TAC_LABEL_END:
    case TAC_LABEL_IF:
      reset_arg_registers();
      fprintf(asm_out, "%s:\n", t->result->name);
      break;

    case TAC_GOTO:
      reset_arg_registers();
      fprintf(asm_out, "  jmp %s\n", t->result->name);
      break;

    case TAC_IFZ:
      reset_arg_registers();
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      fprintf(asm_out, "  mov $1, %%r11\n");
      fprintf(asm_out, "  cmp %%r10, %%r11\n");
      fprintf(asm_out, "  jne %s\n", t->result->name);
      break;

    // --- Functions ---
    case TAC_PARAM: {
      const char *where = get_arg_register();
      if (where[0] == '%') {
        // Register --> memory (valid)
        fprintf(asm_out, "  mov %s, %d(%%rbp)\n", where, t->op1->offset);
      } else {
        // Memory --> memory (not allowed) --> go through a temp register
        fprintf(asm_out, "  mov %s, %%r10\n", where);
        fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->op1->offset);
      }
      break;
    }

    case TAC_ARG: {
      // Prepare argument (registers or stack) --> use get_arg_register() once
      const char *where = get_arg_register();
      if (t->op1->offset == 0) {
        // Immediate
        if (where[0] == '%')
          fprintf(asm_out, "  mov $%d, %s\n", t->op1->value, where);
        else
          fprintf(asm_out, "  movq $%d, %s\n", t->op1->value, where);
      } else {
        // Value at local memory/temporal
        if (where[0] == '%') {
          fprintf(asm_out, "  mov %d(%%rbp), %s\n", t->op1->offset, where);
        } else {
          // Memory --> memory is not allowed; go through a temp register
          fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);
          fprintf(asm_out, "  movq %%r10, %s\n", where);
        }
      }
      break;
    }

    case TAC_CALL:
      reset_arg_registers();
      fprintf(asm_out, "  call %s\n", t->op1->name);
      // t->op1->offset assumed to be count of pushed args (caller
      // responsibility)
      if (t->result)
        fprintf(asm_out, "  mov %%rax, %d(%%rbp)\n", t->result->offset);
      break;

    case TAC_RETURN:
      reset_arg_registers();
      if (t->result) {
        if (t->result->offset == 0)
          fprintf(asm_out, "  mov $%d, %%rax\n", t->result->value);
        else
          fprintf(asm_out, "  mov %d(%%rbp), %%rax\n", t->result->offset);
      }
      fprintf(asm_out, "  leave\n");
      fprintf(asm_out, "  ret\n");

      break;

    case TAC_EXTERN:
      fprintf(asm_out, ".extern %s\n", t->op1->name);
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
