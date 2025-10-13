#include "../headers/main.h"

extern char *yytext;

Scope *scope = NULL;
int debug_flag = 0;
int assembly_flag = 0;
int inter_code_flag = 0;

/**
 * Entry point for the compiler project.
 *
 * This function initializes and sets up all necessary components required
 * before executing the main logic of the compiler.
 * It is responsible for preparing the environment, handling any required
 * configuration, and starting the compilation process.
 *
 * Returns 0 on successful execution
 * Retuns a non-zero value if an error occurs.
 */
int compiler_main(int argc, char *argv[]) {
  scope = malloc(sizeof(Scope));
  scope->tail = NULL;
  initialize_scope();

  char *outfile, *target, *opt, *inputfile;

  if (process_arguments(argc, argv, &outfile, &target, &opt, &inputfile) < 0) {
    usage_message(argv[0]);
    return EXIT_FAILURE;
  }

  // Open source file
  yyin = fopen(inputfile, "r");
  if (!yyin) {
    perror("Error opening input file");
    usage_message(argv[0]);
  }

  // Debug info
  if (debug_flag) {
    printf("[DEBUG] Input file: %s\n", inputfile);
    printf("[DEBUG] Out file: %s\n", outfile);
    if (target)
      printf("[DEBUG] Target: %s\n", target);
    if (opt)
      printf("[DEBUG] Optimizations: %s\n", opt);
  }

  if (target) {
    if (process_target_stage(target) < 0) {
      usage_message(argv[0]);
      return EXIT_FAILURE;
    }
  } else {
    // Normal compilation
    parse_method();
  }

  fclose(yyin);

  // Create output file if specified
  if (create_output_file(outfile) < 0) {
    return EXIT_FAILURE;
  }

  return 0;
}
