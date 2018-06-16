#include <stdio.h>
#include "analyzer.h"

// Convert the input file into lexeme values and export to an output file.
int analyzeSource(char *sourceFile, char *outFile, int options) {
    int i;
    FILE *fin;
    FILE *fout;
    char buffer;
    int singleStatus;
    int returnStatus;

    const int pairedSymbolsCount = 4;
    const int directMappedSymbolsCount = 9;

    SymbolValuePair directMappedSymbols[] = {
        { '+', PLUS },
        { '-', MINUS },
        { '*', MULTIPLY },
        { '(', LEFT_PARENTHESIS },
        { ')', RIGHT_PARENTHESIS },
        { ',', COMMA },
        { '.', PERIOD },
        { ';', SEMICOLON },
        { '=', EQUAL }
    };

    SymbolSymbolPair pairedSymbols[] = {
        { '<', { '>', '=' }, LESS, { NOT_EQUAL, LESS_EQUAL }, 2 },
        { '>', { '=' }, GREATER, { GREATER_EQUAL }, 1 },
        { ':', { '=' }, UNKNOWN, { BECOME }, 1 },
        { '/', { '*' }, SLASH, { COMMENT }, 1 }
    };
        
    if ((fin = fopen(sourceFile, "r")) == NULL) {
        errorMissingFile(sourceFile);
        
        return OP_FAILURE;
    }

    if ((fout = fopen(outFile, "w")) == NULL) {
        errorMissingFile(outFile);

        return OP_FAILURE;
    }

    returnStatus = OP_SUCCESS;

    // Read through the characters in a file and match them to their
    // appropriate lexeme values using handler functions.
    while (fscanf(fin, " %c", &buffer) != EOF) {
        if (isAlphabetic(buffer)) {
            singleStatus = handleLongToken(fin, fout, buffer, IDENTIFIER, IDENTIFIER_LEN); 
        }
        else if (isDigit(buffer)) {
            singleStatus = handleLongToken(fin, fout, buffer, NUMBER, NUMBER_LEN); 
        }
        else {
            for (i = 0; i < directMappedSymbolsCount; i++) {
                if (buffer == directMappedSymbols[i].symbol) {
                    singleStatus = handleDirectMappedSymbol(fout, directMappedSymbols[i].value);
                    break;
                }
            }
            // If the loop didn't terminate early (i.e. the buffer was not
            // a direct-mapped symbol), loop through the paired symbols.
            if (i == directMappedSymbolsCount) {
                for (i = 0; i < pairedSymbolsCount; i++) {
                    if (buffer == pairedSymbols[i].lead) {
                        singleStatus = handlePair(fin, fout, pairedSymbols[i]);
                        break;
                    } 
                }
                // If this loop also didn't terminate early, give up.
                if (i == pairedSymbolsCount) {
                   errorUnknownCharacter(buffer);
                   singleStatus = OP_FAILURE;
                }
            }
        }

        // Set persistent returnStatus to failure if a handler call failed.
        if (singleStatus == OP_FAILURE) {
            returnStatus = OP_FAILURE;

            // If OPTION_SKIP_ERRORS is off, then break the loop.
            if (!(options & OPTION_SKIP_ERRORS)) {
                break;
            }
        }
    }

    // Don't leave files open like a lunatic.
    fclose(fin);
    fclose(fout);

    // If OPTION_SKIP_ERRORS is on, pretend like everything went fine.
    return (options & OPTION_SKIP_ERRORS) ? OP_SUCCESS : returnStatus;
}
