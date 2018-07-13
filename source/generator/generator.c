// Part of Plum by Tiger Sachse.

#include <stdio.h>
#include <stdlib.h>
#include "generator.h"

// Create an IOTunnel to manage the input and output streams of the parser.
IOTunnel *createIOTunnel(char *lexemeFile, char *outFile) {
    IOTunnel *tunnel;

    if ((tunnel = calloc(1, sizeof(IOTunnel))) == NULL) {
        printError(ERROR_OUT_OF_MEMORY);

        return NULL;
    }

    if ((tunnel->fin = fopen(lexemeFile, "r")) == NULL) {
        printError(ERROR_FILE_NOT_FOUND, lexemeFile);
        free(tunnel);

        return NULL;
    }

    if ((tunnel->fout = fopen(outFile, "w")) == NULL) {
        printError(ERROR_FILE_NOT_FOUND, outFile);
        fclose(tunnel->fin);
        free(tunnel);

        return NULL;
    }

    return tunnel;
}

// Load a token from the input stream.
int loadToken(IOTunnel *tunnel) {
    int i;
    char buffer;
    
    if (tunnel == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    // If we've reached the end of the file, then return failure.
    if (fscanf(tunnel->fin, " %d%c", &(tunnel->token), &buffer) == EOF) {
        return SIGNAL_FAILURE;
    }
   
    // If the token wasn't followed by whitespace, then the input file is
    // formatted incorrectly.
    if (!isWhitespace(buffer)) {
        printError(ERROR_ILLEGAL_LEXEME_FORMAT, tunnel->token);

        return SIGNAL_FAILURE;
    }

    // Handle identifiers appropriately, or erase the tokenName for all
    // other tokens.
    if (tunnel->token == LEX_IDENTIFIER) {
        if (handleIdentifier(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        } 
    }
    else {
        tunnel->tokenName[0] = '\0';
    }
   
    // Handle numbers appropriately or default the tokenValue to zero
    // for all other tokens.
    if (tunnel->token == LEX_NUMBER) {
        if (handleNumber(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }    
    }
    else {
        tunnel->tokenValue = 0;
    }

    return SIGNAL_SUCCESS;
}

// Handle identifiers in the lexeme list.
int handleIdentifier(IOTunnel *tunnel) {
    int i;
    char buffer;

    if (tunnel == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    // Absorb an appropriate amount of characters.
    for (i = 0; i < IDENTIFIER_LEN; i++) {

        // If the file ends, adjust the tokenName.
        if (fscanf(tunnel->fin, "%c", tunnel->tokenName + i) == EOF) {
            tunnel->tokenName[i] = '\0';

            // If no tokens were added, then the tokenName is missing.
            if (i == 0) {
                printError(ERROR_MISSING_TOKEN, LEX_IDENTIFIER);

                return SIGNAL_FAILURE;
            }
        }

        // If the most recent character is not an alphanumeric character, then
        // the identifier has ended.
        if (!isAlphanumeric(tunnel->tokenName[i])) {
            fseek(tunnel->fin, -1, SEEK_CUR);

            if (!isWhitespace(tunnel->tokenName[i])) {
                tunnel->tokenName[i] = '\0';
                printError(ERROR_ILLEGAL_IDENTIFIER, tunnel->tokenName);

                return SIGNAL_FAILURE;
            }
            else {
                tunnel->tokenName[i] = '\0';
                break;
            }
        }
    }

    // If, after running out of characters in the loop, there's still more
    // alphanumeric characters, then the token is too long.
    if (fscanf(tunnel->fin, "%c", &buffer) != EOF && isAlphanumeric(buffer)) {
        printError(ERROR_TOKEN_TOO_LONG, tunnel->tokenName);

        return SIGNAL_FAILURE;
    }
    else {
        fseek(tunnel->fin, -1, SEEK_CUR);
    }

    return SIGNAL_SUCCESS;
}

// Handle numbers in the lexeme list.
int handleNumber(IOTunnel *tunnel) {
   
    if (tunnel == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    // If the expected number isn't there, throw a missing token error.
    if (fscanf(tunnel->fin, " %d", &(tunnel->tokenValue)) == EOF) {
       printError(ERROR_MISSING_TOKEN, LEX_NUMBER);

       return SIGNAL_FAILURE;
    }
    else {
        return SIGNAL_SUCCESS;
    }
}

// Destroy the IOTunnel.
void destroyIOTunnel(IOTunnel *tunnel) {
    fclose(tunnel->fin);
    fclose(tunnel->fout);
    free(tunnel);
}

// Compile lexemes from the lexemeFile into usable bytecode.
int compileLexemes(char *lexemeFile, char *outFile, int options) {
    IOTunnel *tunnel;
    SymbolTable *table;

    if ((table = createSymbolTable()) == NULL) {
        return SIGNAL_FAILURE;
    }
    
    if ((tunnel = createIOTunnel(lexemeFile, outFile)) == NULL) {
        return SIGNAL_FAILURE;
    }

    while (loadToken(tunnel) == SIGNAL_SUCCESS) {
        printf("%d %d %s\n", tunnel->token, tunnel->tokenValue, tunnel->tokenName);
    }

    destroyIOTunnel(tunnel);
    destroySymbolTable(table);

    return SIGNAL_SUCCESS;
}
