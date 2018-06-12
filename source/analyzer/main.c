#include <stdio.h>
#include <string.h>
#include "analyzer.h"

// Process all arguments and flags for the program.
int processArguments(int argCount, char **argsVector, char **outFile) {
    int argIndex;
    int options;

    options = 0;

    // For each argument in the vector, if that argument is supported, fiddle
    // the correct bits in the options int.
    for (argIndex = 2; argIndex < argCount; argIndex++) {
        if (strcmp(argsVector[argIndex], "--output-file") == 0 ||
            strcmp(argsVector[argIndex], "-o") == 0) {
           
            // If -o is missing it's trailing argument, scream about it.
            if (argIndex + 1 >= argCount) {
                errorNoArgument("-o");

                return OP_FAILURE;
            }
            // Set outFile to point to the correct spot in the argsVector, and
            // increment argIndex to skip that argument in the next iteration
            // of the loop.
            else {
                *outFile = argsVector[++argIndex];
            }
        }
        else if (strcmp(argsVector[argIndex], "--skip-errors") == 0) {
            options |= OPTION_SKIP_ERRORS;
        }
        else if (strcmp(argsVector[argIndex], "--print-all") == 0 ||
                 strcmp(argsVector[argIndex], "-v") == 0) {
            
            options |= OPTION_PRINT_SOURCE;
            options |= OPTION_PRINT_LEXEME_TABLE;
            options |= OPTION_PRINT_LEXEME_LIST;
        }
        else if (strcmp(argsVector[argIndex], "--print-source") == 0) {
            options |= OPTION_PRINT_SOURCE;
        }
        else if (strcmp(argsVector[argIndex], "--print-lexeme-table") == 0) {
            options |= OPTION_PRINT_LEXEME_TABLE;
        }
        else if (strcmp(argsVector[argIndex], "--print-lexeme-list") == 0) {
            options |= OPTION_PRINT_LEXEME_LIST;
        }
        else {
            errorUnknownArguments();

            return OP_FAILURE;
        }
    }

    return options;
}

// Main entry point of program.
int main(int argCount, char **argsVector) {
    int options;
    char *outFile;
    
    outFile = DEFAULT_OUTFILE;
    
    // If no source file is provided, scream about it.
    if (argCount < 2) {
        errorNoSource();

        return 0;
    }

    // If something went wrong while processing arguments, kill the program.
    if ((options = processArguments(argCount, argsVector, &outFile)) == OP_FAILURE) {
        return 0; 
    }

    // Print the source file if the user wants it.
    if (options & OPTION_PRINT_SOURCE) {
        printSource(argsVector[1]);
        printf("\n");
    }

    // Analyze the provided source and print to an output file.
    if (analyzeSource(argsVector[1], outFile, options) == OP_FAILURE) {
        printf("Something went wrong while analyzing source.\n");

        return 0;
    }

    // Print the lexeme table if the user wants it.
    if (options & OPTION_PRINT_LEXEME_TABLE) {
        printf("\n");
        printLexemeTable(outFile);
        printf("\n");
    }

    // Print the lexeme list if the user wants it.
    if (options & OPTION_PRINT_LEXEME_LIST) {
        printLexemeList(outFile);
        printf("\n");
    }

    return 0;
}
