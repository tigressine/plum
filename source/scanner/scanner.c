// Part of Plum by Tiger Sachse.

#include <stdio.h>
#include "scanner.h"

// Convert the input file into lexeme values and export to an output file.
int scanSource(char *sourceFile, char *outFile, int options) {
    int i;
    FILE *fin;
    FILE *fout;
    char buffer;
    int singleStatus;
    int returnStatus;

    const int pairedSymbolsCount = 4;
    const int directMappedSymbolsCount = 9;

    SymbolValuePair directMappedSymbols[] = {
        { '+',LEX_PLUS },
        { '-',LEX_MINUS },
        { '*',LEX_MULTIPLY },
        { '(',LEX_LEFT_PARENTHESIS },
        { ')',LEX_RIGHT_PARENTHESIS },
        { ',',LEX_COMMA },
        { '.',LEX_PERIOD },
        { ';',LEX_SEMICOLON },
        { '=',LEX_EQUAL }
    };

    SymbolSymbolPair pairedSymbols[] = {
        { '<', { '>', '=' }, LEX_LESS, { LEX_NOT_EQUAL, LEX_LESS_EQUAL }, 2 },
        { '>', { '=' }, LEX_GREATER, { LEX_GREATER_EQUAL }, 1 },
        { ':', { '=' }, LEX_UNKNOWN, { LEX_BECOME }, 1 },
        { '/', { '*' }, LEX_SLASH, { LEX_COMMENT }, 1 }
    };
        
    if ((fin = fopen(sourceFile, "r")) == NULL) {
        printError(ERROR_FILE_NOT_FOUND, sourceFile);

        return SIGNAL_FAILURE;
    }

    if ((fout = fopen(outFile, "w")) == NULL) {
        printError(ERROR_FILE_NOT_FOUND, outFile);
        fclose(fin);

        return SIGNAL_FAILURE;
    }

    returnStatus = SIGNAL_SUCCESS;

    // Read through the characters in a file and match them to their
    // appropriate lexeme values using handler functions.
    while (fscanf(fin, " %c", &buffer) != EOF) {
        if (isAlphabetic(buffer)) {
            singleStatus = handleLongToken(fin, fout, buffer, LEX_IDENTIFIER, IDENTIFIER_LEN); 
        }
        else if (isDigit(buffer)) {
            singleStatus = handleLongToken(fin, fout, buffer, LEX_NUMBER, NUMBER_LEN); 
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
                   printError(ERROR_UNKNOWN_CHARACTER, buffer);
                   singleStatus = SIGNAL_FAILURE;
                }
            }
        }

        // Set persistent returnStatus to failure if a handler call failed.
        if (singleStatus == SIGNAL_FAILURE) {
            returnStatus = SIGNAL_FAILURE;

            // If OPTION_SKIP_ERRORS is off, then break the loop.
            if (!checkOption(&options, OPTION_SKIP_ERRORS)) {
                break;
            }
        }
    }

    // Don't leave files open like a lunatic.
    fclose(fin);
    fclose(fout);

    if (returnStatus != SIGNAL_FAILURE || checkOption(&options, OPTION_SKIP_ERRORS)) {
        if (checkOption(&options, OPTION_PRINT_SOURCE)) {
            printSource(sourceFile);
        }
        if (checkOption(&options, OPTION_PRINT_LEXEME_TABLE)) {
            printLexemeTable(outFile);
        }
        if (checkOption(&options, OPTION_PRINT_LEXEME_LIST)) {
            printLexemeList(outFile);
        }
    }

    // If OPTION_SKIP_ERRORS is on, pretend like everything went fine.
    return checkOption(&options, OPTION_SKIP_ERRORS) ? SIGNAL_SUCCESS : returnStatus;
}
