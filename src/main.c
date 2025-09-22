#include "../headers/main.h"

extern char *yytext;
TablaSimbolos *global_table;

TablaSimbolos *global_table = NULL;
int debug_flag = 0;
int assembly_flag = 0;

int compiler_main(int argc, char *argv[])
{
  global_table = crear_tabla(10);

  char *outfile, *target, *opt, *inputfile;

  if (process_arguments(argc, argv, &outfile, &target, &opt, &inputfile) < 0)
  {
    usage_message(argv[0]);
    return EXIT_FAILURE;
  }

  // Open source file
  yyin = fopen(inputfile, "r");
  if (!yyin)
  {
    perror("Error opening input file");
    usage_message(argv[0]);
  }

  // Debug info
  if (debug_flag)
  {
    printf("[DEBUG] Input file: %s\n", inputfile);
    if (outfile)
      printf("[DEBUG] Out file: %s\n", outfile);
    if (target)
      printf("[DEBUG] Target: %s\n", target);
    if (opt)
      printf("[DEBUG] Optimizations: %s\n", opt);
  }

  if (target && strcmp(target, "scan") == 0)
  {
    // Lexical analysis
    printf("Stage: Scan\n");
    int token;
    while ((token = yylex()) != 0)
    {
      switch (token)
      {
      case V_NUM:
        printf("TOKEN V_NUM: '%s'\n", yytext);
        break;
      case V_TRUE:
        printf("TOKEN V_TRUE: '%s'\n", yytext);
        break;
      case V_FALSE:
        printf("TOKEN V_FALSE: '%s'\n", yytext);
        break;
      case R_EXTERN:
        printf("TOKEN R_EXTERN: '%s'\n", yytext);
        break;
      case R_BOOL:
        printf("TOKEN R_BOOL: '%s'\n", yytext);
        break;
      case R_INTEGER:
        printf("TOKEN R_INTEGER: '%s'\n", yytext);
        break;
      case R_VOID:
        printf("TOKEN R_VOID: '%s'\n", yytext);
        break;
      case R_RETURN:
        printf("TOKEN R_RETURN: '%s'\n", yytext);
        break;
      case R_WHILE:
        printf("TOKEN R_WHILE: '%s'\n", yytext);
        break;
      case R_PROGRAM:
        printf("TOKEN R_PROGRAM: '%s'\n", yytext);
        break;
      case R_IF:
        printf("TOKEN R_IF: '%s'\n", yytext);
        break;
      case R_THEN:
        printf("TOKEN R_THEN: '%s'\n", yytext);
        break;
      case OP_EQ:
        printf("TOKEN OP_EQ: '%s'\n", yytext);
        break;
      case OP_AND:
        printf("TOKEN OP_AND: '%s'\n", yytext);
        break;
      case OP_OR:
        printf("TOKEN OP_OR: '%s'\n", yytext);
        break;
      case ID:
        printf("TOKEN ID: '%s'\n", yytext);
        break;
      default:
        printf("operatorDelimiter %d: '%s'\n", token, yytext);
      }
    }
    printf("EOF\n");
  }
  else if (target && strcmp(target, "parse") == 0)
  {
    // Syntax analysis
    printf("Stage: Parse\n");
    parse_method();
  }
  else if (target && strcmp(target, "codinter") == 0)
  {
    // Intermedium code
    printf("Stage: Intermedium code\n");
  }
  else if (target && strcmp(target, "assembly") == 0)
  {
    // Assembly
    printf("Stage: Assembly\n");
    assembly_flag = 1;
    parse_method();
  }
  else if (target)
  {
    // Target flag set but invalid stage provided
    usage_message(argv[0]);
  }
  else
  {
    // Normal compilation
    parse_method();
  }

  fclose(yyin);

  // Create symbolic link with specified output name
  if (outfile)
  {
    // Remove existing file if it exists
    if (access(outfile, F_OK) == 0)
    {
      unlink(outfile);
    }
    // Create symbolic link with specified name
    if (symlink("c-tds", outfile) != 0)
    {
      perror("Error creating output file");
      return EXIT_FAILURE;
    }
    if (debug_flag)
    {
      printf("[DEBUG] Created output file: %s\n", outfile);
    }
  }

  return 0;
}

void parse_method()
{
  int compiled_with_errors = yyparse();

  if (compiled_with_errors)
    printf("Compilation completed with errors.\n");
  else
    printf("Compilation completed successfuly.\n");
}

void usage_message(const char *prog)
{
  fprintf(stderr, "Usage: %s [flags] file.ctds\n", prog);
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -o <output>        Renames the executable\n");
  fprintf(stderr, "  -target <stage>    Stage: scan | parse | codinter | assembly\n");
  fprintf(stderr, "  -opt [opt]         Optimizations (all, ...)\n");
  fprintf(stderr, "  -debug             Debug info\n");
  exit(EXIT_FAILURE);
}
