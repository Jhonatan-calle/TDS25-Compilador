#include "../headers/assembly_code.h"

int temp_counter = 0;

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

static const char* arg_registers[] = {
    "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"
};

static int next_arg = 0;  // índice del próximo registro libre

// Devuelve el siguiente registro de argumento disponible
const char* get_arg_register() {
    if (next_arg >= 6) {
        // Ya se usaron los 6 registros, los siguientes argumentos deben ir en la pila
        return NULL;
    }
    return arg_registers[next_arg++];
}

// Reinicia el uso de registros de argumentos (después de un call)
void reset_arg_registers() {
    next_arg = 0;
}
/**
 * Assembly util function
 *
 * Called on the top level of the program
 * It constructs and prints a pseudo-assembly recursively
 */
void gen_assembly_code(TAC *head) {
  if (head->op == TAC_ASSIGN)
    printf("section .text:\n");
  for (TAC *t = head; t; t = t->next) {
    switch (t->op) {

    // --- Operaciones aritméticas ---
    case TAC_ADD:
      printf("  mov -%d(%%rbp), %%rax\n", t->op1->offset);
      printf("  add -%d(%%rbp), %%rax\n", t->op2->offset);
      printf("  mov %%rax, -%d(%%rbp)\n", t->result->offset);
      break;

    case TAC_SUB:
      printf("  mov -%d(%%rbp), %%rax\n", t->op1->offset);
      printf("  sub -%d(%%rbp), %%rax\n", t->op2->offset);
      printf("  mov %%rax, -%d(%%rbp)\n", t->result->offset);
      break;

    case TAC_MUL:
      printf("  mov -%d(%%rbp), %%rax\n", t->op1->offset);
      printf("  imul -%d(%%rbp), %%rax\n", t->op2->offset);
      printf("  mov %%rax, -%d(%%rbp)\n", t->result->offset);
      break;

    case TAC_DIV:
      printf("  mov -%d(%%rbp), %%rax\n", t->op1->offset);
      printf("  cqo\n");
      printf("  idiv -%d(%%rbp)\n", t->op2->offset);
      printf("  mov %%rax, -%d(%%rbp)\n", t->result->offset);
      break;

    case TAC_MOD:
      printf("  mov -%d(%%rbp), %%rax\n", t->op1->offset);
      printf("  cqo\n");
      printf("  idiv -%d(%%rbp)\n", t->op2->offset);
      printf("  mov %%rdx, -%d(%%rbp)\n", t->result->offset);
      break;

    // --- Comparaciones ---
    case TAC_LESS:
      printf("  mov -%d(%%rbp), %%rax\n", t->op1->offset);
      printf("  cmp -%d(%%rbp), %%rax\n", t->op2->offset);
      printf("  setl %%al\n");
      printf("  movzbq %%al, %%rax\n");
      printf("  mov %%rax, -%d(%%rbp)\n", t->result->offset);
      break;

    case TAC_GR:
      printf("  mov -%d(%%rbp), %%rax\n", t->op1->offset);
      printf("  cmp -%d(%%rbp), %%rax\n", t->op2->offset);
      printf("  setg %%al\n");
      printf("  movzbq %%al, %%rax\n");
      printf("  mov %%rax, -%d(%%rbp)\n", t->result->offset);
      break;

    case TAC_EQ:
      printf("  mov -%d(%%rbp), %%rax\n", t->op1->offset);
      printf("  cmp -%d(%%rbp), %%rax\n", t->op2->offset);
      printf("  sete %%al\n");
      printf("  movzbq %%al, %%rax\n");
      printf("  mov %%rax, -%d(%%rbp)\n", t->result->offset);
      break;

    // --- Lógicos ---
    case TAC_AND:
      printf("  mov -%d(%%rbp), %%rax\n", t->op1->offset);
      printf("  cmp $0, %%rax\n");
      printf("  setne %%al\n");
      printf("  movzbq %%al, %%rax\n");
      printf("  mov -%d(%%rbp), %%rbx\n", t->op2->offset);
      printf("  cmp $0, %%rbx\n");
      printf("  setne %%bl\n");
      printf("  and %%bl, %%al\n");
      printf("  movzbq %%al, %%rax\n");
      printf("  mov %%rax, -%d(%%rbp)\n", t->result->offset);
      break;

    case TAC_OR:
      printf("  mov -%d(%%rbp), %%rax\n", t->op1->offset);
      printf("  cmp $0, %%rax\n");
      printf("  setne %%al\n");
      printf("  mov -%d(%%rbp), %%rbx\n", t->op2->offset);
      printf("  cmp $0, %%rbx\n");
      printf("  setne %%bl\n");
      printf("  or %%bl, %%al\n");
      printf("  movzbq %%al, %%rax\n");
      printf("  mov %%rax, -%d(%%rbp)\n", t->result->offset);
      break;

    case TAC_NOT:
      printf("  mov -%d(%%rbp), %%rax\n", t->op1->offset);
      printf("  cmp $0, %%rax\n");
      printf("  sete %%al\n");
      printf("  movzbq %%al, %%rax\n");
      printf("  mov %%rax, -%d(%%rbp)\n", t->result->offset);
      break;

    // --- Unarios ---
    case TAC_NEG:
      printf("  mov -%d(%%rbp), %%rax\n", t->op1->offset);
      printf("  neg %%rax\n");
      printf("  mov %%rax, -%d(%%rbp)\n", t->result->offset);
      break;

    // --- Asignación ---
    case TAC_ASSIGN:
      if(t->op1->offset == 0)
        printf("  mov $%d, %%rax\n", t->op1->valor);
      else
        printf("  mov -%d(%%rbp), %%rax\n", t->op1->offset);

      printf("  mov %%rax, -%d(%%rbp)\n", t->result->offset);
      break;

    // --- Control de flujo ---
    case TAC_LABEL:
      printf("%s:\n", t->result->nombre);
      printf("  enter $(8 * %d) \n", t->result->offset);
      break;

    case TAC_GOTO:
      printf("  jmp %s\n", t->result->nombre);
      break;

    case TAC_IFZ:
      printf("  mov -%d(%%rbp), %%rax\n", t->op1->offset);
      printf("  cmp $0, %%rax\n");
      printf("  je %s\n", t->result->nombre);
      break;

    // --- Funciones ---
    case TAC_PARAM:
      printf("  move %s, -%d(%%rpb)\n", get_arg_register(),t->op1->offset);
      break;

    case TAC_ARG:
      printf("  push -%d(%%rbp)\n", t->op1->offset);
      break;

    case TAC_CALL:
      reset_arg_registers();
      printf("  call %s\n", t->result->nombre);
      printf("  add $%d, %%rsp\n", t->op1 ? t->op1->offset * 8 : 0); // limpiar args
      printf("  mov %%rax, -%d(%%rbp)\n", t->result->offset);
      break;

    case TAC_RETURN:
      reset_arg_registers();  
      if (t->result)
        printf("  mov -%d(%%rbp), %%rax\n", t->result->offset);
      printf("  leave\n");
      printf("  ret\n");
      break;

    // --- I/O y externos ---
    case TAC_PRINT:
      printf("  mov -%d(%%rbp), %%rdi\n", t->op1->offset);
      printf("  call print_int\n");
      break;

    case TAC_EXTERN:
      printf("  extern \n");
      break;

    case TAC_UNKNOWN:
    default:
      printf("  ; unknown TAC op %d\n", t->op);
      break;
    }
  }
}
