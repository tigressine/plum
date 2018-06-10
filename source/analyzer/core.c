#include <stdio.h>
#include "analyzer.h"

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

            // If OPTION_SKIP_ERRORS is off, then break the loop.
            if (!(options & OPTION_SKIP_ERRORS)) {
                break;
            }
        }
    }

    // Don't leave files open like a lunatic.
    fclose(fin);
    fclose(fout);
    
    return returnStatus;
}
