#include "../headers/utils.h"
#include "../headers/main.h"
#include "../syntax.tab.h"

extern char *yytext;

int debug = 0;

int compiler_main(int argc, char *argv[])
{
  char *outfile = NULL;
  char *target = NULL;
  char *opt = NULL;

  char *inputfile = NULL;

  // Cycle the arguments
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "-output") == 0)
    {
      if (i + 1 < argc)
      {
        outfile = argv[++i]; // Takes the next as outfile
      }
      else
      {
        fprintf(stderr, "Error: missing argument after -o\n");
        usage(argv[0]);
      }
    }
    else if (strcmp(argv[i], "-target") == 0 || strcmp(argv[i], "-t") == 0)
    {
      if (i + 1 < argc)
      {
        target = argv[++i];
      }
      else
      {
        fprintf(stderr, "Error: missing argument after -target\n");
        usage(argv[0]);
      }
    }
    else if (strcmp(argv[i], "-opt") == 0)
    {
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        opt = argv[++i]; // It can be 'all' or another
      }
      else
      {
        opt = "all"; // Assumming 'all' if nothing provided
      }
    }
    else if (strcmp(argv[i], "-debug") == 0 || strcmp(argv[i], "-d") == 0)
    {
      debug = 1;
    }
    else
    {
      // If its not an option, assumming its the input file
      inputfile = argv[i];
    }
  }

  if (!inputfile)
  {
    usage(argv[0]);
    return EXIT_FAILURE;
  }

  // Open source file
  yyin = fopen(inputfile, "r");
  if (!yyin)
  {
    perror("Error opening input file");
    usage(argv[0]);
  }

  // Debug info
  if (debug)
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
    while ((token = yylex()) != 0) {
        switch (token) {
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
    yyparse();
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
  }
  else if (target)
  {
    // Target flag but invalid stage provided
    usage(argv[0]);
  }
  else
  {
    // Normal compilation completed
    yyparse();
    printf("Compilation completed.\n");
  }

  fclose(yyin);
  return 0;
}

void usage(const char *prog)
{
  fprintf(stderr, "Usage: %s [flags] file.ctds\n", prog);
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -o <output>        Renames the executable\n");
  fprintf(stderr, "  -target <stage>    Stage: scan | parse | codinter | assembly\n");
  fprintf(stderr, "  -opt [opt]         Optimizations (all, ...)\n");
  fprintf(stderr, "  -debug             Debug info\n");
  exit(EXIT_FAILURE);
}
