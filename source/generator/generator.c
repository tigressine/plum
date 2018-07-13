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

    tunnel->status = SIGNAL_SUCCESS;

    return tunnel;
}

// Load a token from the input stream.
void loadToken(IOTunnel *tunnel) {
    int i;
    char buffer;
    
    if (tunnel == NULL) {
        printError(ERROR_NULL_CHECK);
        
        return;
    }

    if (fscanf(tunnel->fin, " %d%c", &(tunnel->token), &buffer) == EOF) {
        tunnel->status = SIGNAL_EOF;

        return;
    }
  
    // If the token wasn't followed by whitespace, then the input file is
    // formatted incorrectly.
    if (!isWhitespace(buffer)) {
        printError(ERROR_ILLEGAL_LEXEME_FORMAT, tunnel->token);
        tunnel->status = SIGNAL_FAILURE;
        
        return;
    }

    // Handle identifiers appropriately, or erase the tokenName for all
    // other tokens.
    if (tunnel->token == LEX_IDENTIFIER) {
        handleIdentifier(tunnel);
        if (tunnel->status == SIGNAL_FAILURE) {
            return;
        } 
    }
    else {
        tunnel->tokenName[0] = '\0';
    }
   
    // Handle numbers appropriately or default the tokenValue to zero
    // for all other tokens.
    if (tunnel->token == LEX_NUMBER) {
        if (tunnel->status == SIGNAL_FAILURE) {
            return;
        }    
    }
    else {
        tunnel->tokenValue = 0;
    }

    tunnel->status = SIGNAL_SUCCESS;
}

// Handle identifiers in the lexeme list.
void handleIdentifier(IOTunnel *tunnel) {
    int i;
    char buffer;

    if (tunnel == NULL) {
        printError(ERROR_NULL_CHECK);

        return;
    }

    // Absorb an appropriate amount of characters.
    for (i = 0; i < IDENTIFIER_LEN; i++) {

        // If the file ends, adjust the tokenName.
        if (fscanf(tunnel->fin, "%c", tunnel->tokenName + i) == EOF) {
            tunnel->tokenName[i] = '\0';
            // If no tokens were added, then the tokenName is missing.
            if (i == 0) {
                printError(ERROR_MISSING_TOKEN, LEX_IDENTIFIER);
                tunnel->status = SIGNAL_FAILURE;
                
                return;
            }

            tunnel->status = SIGNAL_EOF;

            return;
        }

        // If the most recent character is not an alphanumeric character, then
        // the identifier has ended.
        if (!isAlphanumeric(tunnel->tokenName[i])) {
            fseek(tunnel->fin, -1, SEEK_CUR);

            if (!isWhitespace(tunnel->tokenName[i])) {
                tunnel->tokenName[i] = '\0';
                printError(ERROR_ILLEGAL_IDENTIFIER, tunnel->tokenName);
                tunnel->status = SIGNAL_FAILURE;

                return;
            }
            else {
                if (i == 0) {
                    printError(ERROR_MISSING_TOKEN, LEX_IDENTIFIER);
                    tunnel->status = SIGNAL_FAILURE;

                    return;
                }
                tunnel->tokenName[i] = '\0';
                break;
            }
        }
    }

    // If, after running out of characters in the loop, there's still more
    // alphanumeric characters, then the token is too long.
    if (fscanf(tunnel->fin, "%c", &buffer) != EOF && isAlphanumeric(buffer)) {
        printError(ERROR_TOKEN_TOO_LONG, tunnel->tokenName);
        tunnel->status = SIGNAL_FAILURE;

        return;
    }
    else {
        fseek(tunnel->fin, -1, SEEK_CUR);
    }

    tunnel->status = SIGNAL_SUCCESS;

    return;
}

// Handle numbers in the lexeme list.
void handleNumber(IOTunnel *tunnel) {
   
    if (tunnel == NULL) {
        printError(ERROR_NULL_CHECK);

        return;
    }

    // If the expected number isn't there, throw a missing token error.
    if (fscanf(tunnel->fin, " %d", &(tunnel->tokenValue)) == EOF) {
       printError(ERROR_MISSING_TOKEN, LEX_NUMBER);
       tunnel->status = SIGNAL_FAILURE;

       return;
    }
    else {
        tunnel->status = SIGNAL_SUCCESS;
        
        return;
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
    int returnValue;
    IOTunnel *tunnel;
    SymbolTable *table;

    if ((table = createSymbolTable()) == NULL) {
        return SIGNAL_FAILURE;
    }
    
    if ((tunnel = createIOTunnel(lexemeFile, outFile)) == NULL) {
        return SIGNAL_FAILURE;
    }

    returnValue = classProgram(tunnel, table); 

    destroyIOTunnel(tunnel);
    destroySymbolTable(table);

    return returnValue;
}
