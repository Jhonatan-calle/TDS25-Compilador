#include "../headers/main.h"

TablaSimbolos *global_table = NULL;
int debug_flag = 0;
int assembly_flag = 0;

int compiler_main(int argc, char *argv[]) {
  global_table = crear_tabla(10);

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
    if (outfile)
      printf("[DEBUG] Out file: %s\n", outfile);
    if (target)
      printf("[DEBUG] Target: %s\n", target);
    if (opt)
      printf("[DEBUG] Optimizations: %s\n", opt);
  }

  if (target) {
    if (process_target_stage(target, inputfile) < 0) {
      usage_message(argv[0]);
      return EXIT_FAILURE;
    }
  } else {
    // Normal compilation
    parse_method();
  }

  fclose(yyin);

  // Create symbolic link with specified output name
  if (outfile) {
    // Remove existing file if it exists
    if (access(outfile, F_OK) == 0) {
      unlink(outfile);
    }
    // Create symbolic link with specified name
    if (symlink("c-tds", outfile) != 0) {
      perror("Error creating output file");
      return EXIT_FAILURE;
    }
    if (debug_flag) {
      printf("[DEBUG] Created output file: %s\n", outfile);
    }
  }

  return 0;
}
