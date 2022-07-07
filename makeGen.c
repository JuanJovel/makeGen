/**
 * makeGen is a program that generates a Makefile for a given number of source
 * files. It generates a simple make file with the user-specified CFLAGS and
 * source files. The makefile includes two rules "all" and "clean". The "all"
 * rule compiles all the source files into a single ELF executable. The "clean"
 * rule removes the executable.
 *
 * Sample usage:
 * Given the following source files:
 *     file1.c  file2.c  file3.c
 * and the following CFLAGS:
 *    -Wall -g -O0
 * and the desired executable name:
 *   my_program
 * makeGen can be invoked as follows:
 *   makeGen myProgram -f -Wall -g -O0 -s file1.c file2.c file3.c
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Some macros to make the code more readable. */
#define MIN_ARGS 4
#define FLAG_NOT_FOUND -1
#define CFLAGS_FLAG_LOCATION 2
#define MAKEFILE_NAME "Makefile"
#define CFLAGS_FLAG "-f"
#define SOURCE_FLAG "-s"
#define COMPILER_FLAG "-cc"

/** Helper function declarations. */
static void printUsage();
static void validateInvocation(char **argv);
__attribute__((deprecated, unused)) static int findSourceFlag(int argc,
                                                              char **argv);
static bool makeFileExists();
static void printHeader(FILE *makeFile);
static void printRules(FILE *makeFile, char *executableName);
static void alertSuccess();
static void findFlags(int argc, char **argv, int *sourceFlagIdx,
                      int *compilerFlagIdx);

/**
 * Main function for make file generator.
 */
int main(int argc, char **argv) {

  // Check for correct number of arguments.
  if (argc == 1 || argc < MIN_ARGS) {
    printUsage();
    return 0;
  }

  // Validate the invocation.
  validateInvocation(argv);

  int sourceFlagIdx, compilerFlagIdx;

  // Find the flags, if they exist.
  findFlags(argc, argv, &sourceFlagIdx, &compilerFlagIdx);
  char *compiler;

  // If the compiler flag is found, set the compiler.
  if (compilerFlagIdx != FLAG_NOT_FOUND && compilerFlagIdx + 1 < argc) {
    // The compiler flag is present the compiler must be specified in the next
    // argument.
    compiler = argv[compilerFlagIdx + 1];
  }

  // If the compiler flag is not found, or the compiler is not specified, set
  // the compiler to gcc.
  else {
    compiler = "gcc";
  }

  // If there are no source files, exit.
  if (sourceFlagIdx == FLAG_NOT_FOUND) {
    printf("Invalid invocation.\n");
    printf("Error: No source files flag \"-s\" found.\n");
    printUsage();
    return 1;
  }

  // If the makefile already exists, exit.
  if (makeFileExists()) {
    printf("Unable to create makefile:\n");
    printf("makeFile already exists in this directory.\n");
    return 1;
  }

  // Gather the executable name.
  char *executableName = argv[1];

  // Create the makefile.
  FILE *makeFile = fopen(MAKEFILE_NAME, "w+");

  // Check if the makefile was created sucessfully.
  if (makeFile == NULL) {
    printf("FATAL ERROR:\n");
    printf("Unable to create makefile:\n");
    printf("makeFile could not be created.\n");
    return 1;
  }

  // Write the makefile:

  // Print the header.
  printHeader(makeFile);

  // Print compiler and CFLAGS definitions
  fprintf(makeFile, "CC=%s\n", compiler);
  fprintf(makeFile, "CFLAGS=");

  // Print the user specified CFLAGS.
  for (int i = CFLAGS_FLAG_LOCATION + 1; i < sourceFlagIdx; i++) {
    fprintf(makeFile, "%s ", argv[i]);
  }
  fprintf(makeFile, "\n");

  // Print the source files.
  fprintf(makeFile, "TARGETS=");
  int sourceEnd = compilerFlagIdx == FLAG_NOT_FOUND ? argc : compilerFlagIdx;
  for (int i = sourceFlagIdx + 1; i < sourceEnd; i++) {
    fprintf(makeFile, "%s ", argv[i]);
  }

  // Print the automatically generated rules.
  printRules(makeFile, executableName);

  // Close the makefile.
  fclose(makeFile);

  // Alert the user that the makefile was created.
  alertSuccess();

  return 0;
}

/**
 * Validates the invocation of the program.
 * @param argv The invocation arguments.
 */
static void validateInvocation(char **argv) {

  // Argument at index 2 should always be 2 chars long
  // and be the -f flag for a valid invocation.
  if (strcmp(argv[CFLAGS_FLAG_LOCATION], CFLAGS_FLAG) != 0) {
    printf("Invalid invocation.\n");
    printf("Error: No flags found.\n");
    printUsage();
    exit(1);
  }
}

static void findFlags(int argc, char **argv, int *sourceFlagIdx,
                      int *compilerFlagIdx) {
  *sourceFlagIdx = FLAG_NOT_FOUND;
  *compilerFlagIdx = FLAG_NOT_FOUND;
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], SOURCE_FLAG) == 0 && *sourceFlagIdx == FLAG_NOT_FOUND) {
      *sourceFlagIdx = i;
    }
    if (strcmp(argv[i], COMPILER_FLAG) == 0 &&
        *sourceFlagIdx != FLAG_NOT_FOUND &&
        *compilerFlagIdx == FLAG_NOT_FOUND) {
      *compilerFlagIdx = i;
    }
  }
}

/**
 * @deprecated
 * Find the source flag in the given arguments, if it exists.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return The index of the source flag, or -1 if it does not exist.
 */
__attribute__((deprecated, unused)) static int findSourceFlag(int argc,
                                                              char **argv) {

  // Get the source flag location.
  int sourceFlagIdx = FLAG_NOT_FOUND;
  for (int i = 3; i < argc; i++) {
    if (strcmp(argv[i], SOURCE_FLAG) == 0) {
      sourceFlagIdx = i;
      break;
    }
  }

  return sourceFlagIdx;
}

/**
 * Checks if the makefile already exists in the current directory.
 * @return True if the makefile exists, false otherwise.
 */
static bool makeFileExists() { return access(MAKEFILE_NAME, F_OK) != -1; }

/**
 * Prints a correct usage message to stdout.
 */
static void printUsage() {
  printf("Usage:\n");
  printf("makeGen {executableName} -f {CFLAGS} -s {SOURCE FILES} [-cc {desired "
         "compiler}]\n");
  printf("Fields in brackets are optional.\n");
}

/**
 * Prints the header to the makefile.
 */
static void printHeader(FILE *makeFile) {
  fprintf(makeFile, "# Automatically generated makefile\n");
  fprintf(makeFile, "# Generated using makeGen by Juan Jovel\n");
  fprintf(makeFile, "\n");
}

/**
 * Prints the automatically generated rules to the makefile.
 */
static void printRules(FILE *makeFile, char *executableName) {
  fprintf(makeFile, "\n\n");

  fprintf(makeFile, "all:\n");
  fprintf(makeFile, "\t$(CC) $(CFLAGS) -o %s $(TARGETS)\n", executableName);

  fprintf(makeFile, "\n");

  fprintf(makeFile, "clean:\n");
  fprintf(makeFile, "\trm -f %s\n", executableName);

  fprintf(makeFile, "\n");

  fprintf(makeFile, "# End automatically generated makeFile\n");
}

/**
 * Alerts the user that the makefile was succesfully created.
 */
static void alertSuccess() { printf("Successfully created makefile.\n"); }
