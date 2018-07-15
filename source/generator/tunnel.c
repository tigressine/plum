// Part of Plum by Tiger Sachse.

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

    if ((tunnel->queue = createInstructionQueue()) == NULL) {
        fclose(tunnel->fin);
        fclose(tunnel->fout);
        free(tunnel);

        return NULL;
    }

    tunnel->status = SIGNAL_SUCCESS;

    return tunnel;
}

// Send a given instruction either to file or into the queue.
int emitInstruction(IOTunnel *tunnel, Instruction instruction, int useQueue) {
    if (tunnel == NULL || tunnel->queue == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    if (useQueue == SIGNAL_TRUE) {
        if (enqueueInstruction(tunnel->queue, instruction) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        } 
    }
    else {
        if (fprintf(tunnel->fout, "%d %d %d %d\n",
                    instruction.opCode,
                    instruction.RField,
                    instruction.LField,
                    instruction.MField) <= 0) {
            //printError(ERROR_FAILED_WRITING_TO_FILE);
            
            return SIGNAL_FAILURE;
        }
        else {
            tunnel->programCounter++;

            return SIGNAL_SUCCESS;
        }
    }
}

// Emit all of the instructions in the queue in order, until empty.
int emitInstructions(IOTunnel *tunnel) {
    int returnValue;
    QueueNode *current;

    if (tunnel == NULL || tunnel->queue == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
  
    returnValue = SIGNAL_SUCCESS;

    current = tunnel->queue->head;
    // Emit each instruction, starting at the head of the queue. Delete
    // the nodes along the way.
    while (current != NULL) { 
        // If any call to emitInstruction fails, then change the returnValue to
        // failure. Note that this doesn't just return failure immediately, else
        // the rest of the queue would never get deleted, resulting in a memory leak.
        if (emitInstruction(tunnel, current->instruction, SIGNAL_FALSE) == SIGNAL_FAILURE) {
            returnValue = SIGNAL_FAILURE;
        }

        current = current->next;
        tunnel->programCounter++;
    }

    clearInstructionQueue(tunnel->queue);

    return returnValue;
}

int setConstants(IOTunnel *tunnel, SymbolTable *table) {
    TableNode *current;
    Instruction instruction;

    if (tunnel == NULL || table == NULL || table->head == NULL) {
        printError(ERROR_NULL_CHECK);
        
        return SIGNAL_FAILURE;
    }

    current = table->head;
    while (current != NULL) {
        if (current->symbol.type == LEX_CONST) {
            setInstruction(&instruction, LIT, 0, 0, current->symbol.value);
            if (emitInstruction(tunnel, instruction, SIGNAL_FALSE) == SIGNAL_FAILURE) {

                return SIGNAL_FAILURE;
            }
            setInstruction(&instruction, STO, 0, 0, current->symbol.address);
            if (emitInstruction(tunnel, instruction, SIGNAL_FALSE) == SIGNAL_FAILURE) {

                return SIGNAL_FAILURE;
            }
        }
        current = current->next;
    }

    return SIGNAL_SUCCESS;
}

// Load a token from the input stream.
int loadToken(IOTunnel *tunnel) {
    int i;
    char buffer;
    
    if (tunnel == NULL) {
        printError(ERROR_NULL_CHECK);
        
        return SIGNAL_FAILURE;
    }

    if (tunnel->status == SIGNAL_EOF) {
        printError(ERROR_END_OF_FILE);
        tunnel->status = SIGNAL_FAILURE;
        
        return SIGNAL_FAILURE;
    }

    if (fscanf(tunnel->fin, " %d%c", &(tunnel->token), &buffer) == EOF) {
        tunnel->status = SIGNAL_EOF;
    }
 
    // If the token wasn't followed by whitespace, then the input file is
    // formatted incorrectly. There is a slight bug in this code: if we are nearly
    // at the end of the file then the buffer could be loaded with some external
    // junk value. This is usually zero but not always. In this instance it seems
    // relatively inconsequential.
    if (!isWhitespace(buffer)) {
        printError(ERROR_ILLEGAL_LEXEME_FORMAT, tunnel->token);
        tunnel->status = SIGNAL_FAILURE;
        
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

    if (tunnel->status != SIGNAL_EOF) {
        tunnel->status = SIGNAL_SUCCESS;
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

    if (tunnel->status == SIGNAL_EOF) {
        printError(ERROR_MISSING_TOKEN, LEX_IDENTIFIER);
        
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
                tunnel->status = SIGNAL_FAILURE;
                
                return SIGNAL_FAILURE;
            }

            tunnel->status = SIGNAL_EOF;
        }

        // If the most recent character is not an alphanumeric character, then
        // the identifier has ended.
        if (!isAlphanumeric(tunnel->tokenName[i])) {
            fseek(tunnel->fin, -1, SEEK_CUR);

            if (!isWhitespace(tunnel->tokenName[i])) {
                tunnel->tokenName[i] = '\0';
                printError(ERROR_ILLEGAL_IDENTIFIER, tunnel->tokenName);
                tunnel->status = SIGNAL_FAILURE;

                return SIGNAL_FAILURE;
            }
            else {
                if (i == 0) {
                    printError(ERROR_MISSING_TOKEN, LEX_IDENTIFIER);
                    tunnel->status = SIGNAL_FAILURE;

                    return SIGNAL_FAILURE;
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

        return SIGNAL_FAILURE;
    }
    else {
        fseek(tunnel->fin, -1, SEEK_CUR);
    }

    tunnel->status = SIGNAL_SUCCESS;

    return SIGNAL_SUCCESS;
}

// Handle numbers in the lexeme list.
int handleNumber(IOTunnel *tunnel) {
   
    if (tunnel == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
    
    if (tunnel->status == SIGNAL_EOF) {
        printError(ERROR_MISSING_TOKEN, LEX_IDENTIFIER);
        
        return SIGNAL_FAILURE;
    }

    // If the expected number isn't there, throw a missing token error.
    if (fscanf(tunnel->fin, " %d", &(tunnel->tokenValue)) == EOF) {
       printError(ERROR_MISSING_TOKEN, LEX_NUMBER);
       tunnel->status = SIGNAL_FAILURE;

       return SIGNAL_FAILURE;
    }
    else {
        tunnel->status = SIGNAL_SUCCESS;
        
        return SIGNAL_SUCCESS;
    }
}

// Destroy the IOTunnel.
void destroyIOTunnel(IOTunnel *tunnel) {
    if (tunnel == NULL) {
        return;
    }

    fclose(tunnel->fin);
    fclose(tunnel->fout);
    destroyInstructionQueue(tunnel->queue);
    free(tunnel);
}
