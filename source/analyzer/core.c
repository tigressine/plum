#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "analyzer.h"

// Main entry point of program.
int main(int argsCount, char **argsVector) {
    int argIndex;
    char *outFile;
    
    // If no source file is provided, scream about it.
    if (argsCount < 2) {
        printf("ERROR: Please pass a PL/0 source file as your first argument.\n");

        return 0;
    }

    // Will trigger use of DEFAULT_OUTFILE later in code if no outFile is
    // specified in flags.
    outFile = NULL;

    // Parse arguments passed after first argument for known flags.
    for (argIndex = 2; argIndex < argsCount; argIndex++) {
        // Optional flag '-o' for outFile specification.
        if (strcmp(argsVector[argIndex], "-o") == 0) {
            // If There's no argument after the flag, scream about it.
            if (argIndex + 1 >= argsCount) {
                printf("ERROR: No argument provided after '-o' flag.\n");

                return 0;
            }
            // Otherwise point outFile at desired output file vector from user.
            else {
                outFile = argsVector[argIndex + 1];
                // Skip parsing the argument following the '-o' flag.
                argIndex++;
            }
        }
        // If an argument can't be understood, scream about it.
        else {
            printf("ERROR: Unknown arguments after initial argument.\n");

            return 0;
        }
    }
    
    // Analyze the provided source and print to an output file.
    if (analyzeSource(argsVector[1],
                      (outFile == NULL) ? DEFAULT_OUTFILE : outFile) == OP_FAILURE) {
        printf("Something went wrong while analyzing source.\n");

        return 0;
    }

    return 0;
}

// Convert the input file into lexeme values and export to an output file.
int analyzeSource(char *sourceFile, char *outFile) {
    FILE *fin;
    FILE *fout;
    char buffer;

    // If the input file can't be opened, return failure.
    if ((fin = fopen(sourceFile, "r")) == NULL) {
        return OP_FAILURE;
    }

    // If the output file can't be created, return failure.
    if ((fout = fopen(outFile, "w")) == NULL) {
        return OP_FAILURE;
    }

    // Read through the characters in a file and match them to their
    // appropriate lexeme values using handler functions.
    while (fscanf(fin, " %c", &buffer) != EOF) {
        switch (buffer) {
            case '+': handleDirectMappedSymbol(fout, PLUS); break;
            case '-': handleDirectMappedSymbol(fout, MINUS); break;
            case '*': handleDirectMappedSymbol(fout, MULTIPLY); break;
            case '(': handleDirectMappedSymbol(fout, LEFT_PARENTHESIS); break;
            case ')': handleDirectMappedSymbol(fout, RIGHT_PARENTHESIS); break;
            case ',': handleDirectMappedSymbol(fout, COMMA); break;
            case '.': handleDirectMappedSymbol(fout, PERIOD); break;
            case ';': handleDirectMappedSymbol(fout, SEMICOLON); break;
            case '<': handlePair(fin, fout, buffer, '=', LESS_EQUAL, LESS); break;
            case '>': handlePair(fin, fout, buffer, '=', GREATER_EQUAL, GREATER); break;
            case ':': handlePair(fin, fout, buffer, '=', BECOME, UNKNOWN); break;
            case '/': handlePair(fin, fout, buffer, '*', COMMENT, SLASH); break;
            default:
                if (isAlphabetic(buffer)) {
                    handleLongToken(fin, fout, buffer, IDENTIFIER, IDENTIFIER_LEN); 
                }
                else if (isDigit(buffer)) {
                    handleLongToken(fin, fout, buffer, NUMBER, NUMBER_LEN); 
                }
                else {
                    skipUnknownCharacter(buffer);
                }

                break;
        }
    }

    // Don't leave files open like a lunatic.
    fclose(fin);
    fclose(fout);
    
    return OP_SUCCESS;
}

// Skip past a comment in the source file.
int skipComment(FILE *f) {
    char buffer;

    // Make sure we aren't passing NULL to fscanf.
    if (f == NULL) {
        return OP_FAILURE;
    }

    // Assumes that the "/*" characters have already been consumed by the
    // file pointer. Scans through the file until "*/" is found or the end
    // of the file is reached.
    while(fscanf(f, "%c", &buffer) != EOF) {
        if (buffer == '*') {
            fscanf(f, "%c", &buffer);
            if (buffer == '/') {
                return OP_SUCCESS;
            }
        }
    }

    return OP_FAILURE;
}

// Print message about skipped character.
void skipUnknownCharacter(char unknown) {
    printf("Skipping unknown character '%c'.\n", unknown);
}
