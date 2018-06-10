#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "analyzer.h"

// Main entry point of program.
int main(int argsCount, char **argsVector) {
    int argIndex;
    
    char *outFile = DEFAULT_OUTFILE;
    
    // If no source file is provided, scream about it.
    if (argsCount < 2) {
        printf("ERROR: Please pass a PL/0 source file as your first argument.\n");

        return 0;
    }

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

    printSource(argsVector[1]);
    printf("\n");

    // Analyze the provided source and print to an output file.
    if (analyzeSource(argsVector[1], outFile, 0) == OP_FAILURE) {
        printf("Something went wrong while analyzing source.\n");
    }

    printLexemeTable(outFile);
    printf("\n");
    printLexemeList(outFile);
    printf("\n");

    return 0;
}

// Convert the input file into lexeme values and export to an output file.
int analyzeSource(char *sourceFile, char *outFile, int options) {
    int i;
    FILE *fin;
    FILE *fout;
    int status;
    char buffer;
    int returnStatus;

    SymbolValuePair directMappedSymbols[] = {
        { '+', PLUS },
        { '-', MINUS },
        { '*', MULTIPLY },
        { '(', LEFT_PARENTHESIS },
        { ')', RIGHT_PARENTHESIS },
        { ',', COMMA },
        { '.', PERIOD },
        { ';', SEMICOLON }
    };

    SymbolSymbolPair pairedSymbols[] = {
        { '<', '=', LESS, LESS_EQUAL },
        { '>', '=', GREATER, GREATER_EQUAL },
        { ':', '=', UNKNOWN, BECOME },
        { '/', '*', SLASH, COMMENT }
    };
        
    if ((fin = fopen(sourceFile, "r")) == NULL) {
        return OP_FAILURE;
    }

    if ((fout = fopen(outFile, "w")) == NULL) {
        return OP_FAILURE;
    }

    returnStatus = OP_SUCCESS;

    // Read through the characters in a file and match them to their
    // appropriate lexeme values using handler functions.
    while (fscanf(fin, " %c", &buffer) != EOF) {
        if (isAlphabetic(buffer)) {
            status = handleLongToken(fin, fout, buffer, IDENTIFIER, IDENTIFIER_LEN); 
        }
        else if (isDigit(buffer)) {
            status = handleLongToken(fin, fout, buffer, NUMBER, NUMBER_LEN); 
        }
        else {
            for (i = 0; i < 8; i++) {
                if (buffer == directMappedSymbols[i].symbol) {
                    status = handleDirectMappedSymbol(fout, directMappedSymbols[i].value);
                    break;
                }
            }
            if (i == 8) {//
                for (i = 0; i < 4; i++) {
                    if (buffer == pairedSymbols[i].lead) {
                        status = handlePair(fin, fout, pairedSymbols[i]);
                        break;
                    } 
                }
            }
            if (i == 4) {//
               errorUnknownCharacter(buffer);
               status = OP_FAILURE;
            }
        }

        // Set persistent returnStatus to failure if a handler call failed.
        if (status == OP_FAILURE) {
            returnStatus = OP_FAILURE;

            // If FAIL_ON_ERROR is set, then break the loop.
            if (options & OPTION_FAIL_ON_ERROR) {
                break;
            }
        }
    }

    // Don't leave files open like a lunatic.
    fclose(fin);
    fclose(fout);
    
    return returnStatus;
}

// Skip past a comment in the source file.
int skipComment(FILE *f) {
    char buffer;

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

// Print error alerting the user of an unknown character.
void errorUnknownCharacter(char unknown) {
    printf("ERROR: Unknown character '%c'.\n", unknown);
}

// Print error explaining that the provided token is too long.
void errorTokenTooLong(char *token, int length) {
    printf("ERROR: Token beginning with '%s' is too long. Maximum length %d.\n", token, length);
}

// Print error upon detection of an identifier starting with a digit.
void errorBadIdentifier(char *token) {
    printf("ERROR: Identifier starting with '%s' cannot start with a digit.\n", token);
}
