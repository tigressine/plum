// Part of Plum by Tiger Sachse.

#include <stdlib.h>
#include "generator.h"

// Create an IOTunnel to manage the input and output streams of the parser.
IOTunnel *createIOTunnel(char *lexemeFile, char *outFile) {
    IOTunnel *tunnel;

    // Create the tunnel container.
    if ((tunnel = calloc(1, sizeof(IOTunnel))) == NULL) {
        printError(ERROR_OUT_OF_MEMORY);

        return NULL;
    }

    // Attempt to open the input file.
    if ((tunnel->fin = fopen(lexemeFile, "r")) == NULL) {
        printError(ERROR_FILE_NOT_FOUND, lexemeFile);
        free(tunnel);

        return NULL;
    }

    // Attempt to open the output file.
    if ((tunnel->fout = fopen(outFile, "w")) == NULL) {
        printError(ERROR_FILE_NOT_FOUND, outFile);
        fclose(tunnel->fin);
        free(tunnel);

        return NULL;
    }

    // Create an empty instruction queue used for nested statements.
    if ((tunnel->queue = createInstructionQueue()) == NULL) {
        fclose(tunnel->fin);
        fclose(tunnel->fout);
        free(tunnel);

        return NULL;
    }

    // Set the tunnel's internal status to success.
    tunnel->status = SIGNAL_SUCCESS;

    return tunnel;
}

// Send a given instruction either to file or into the queue.
int emitInstruction(IOTunnel *tunnel, Instruction instruction, int nestedDepth) {
    if (tunnel == NULL || tunnel->queue == NULL) {
        printError(ERROR_NULL_POINTER);

        return SIGNAL_FAILURE;
    }

    // If this is a nested instruction, then it goes into the queue.
    if (nestedDepth > 0) {
        if (enqueueInstruction(tunnel->queue, instruction) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        } 
    }

    // Else the instruction is printed to the output file.
    else {
        if (fprintf(tunnel->fout, "%d %d %d %d\n",
                    instruction.opCode,
                    instruction.RField,
                    instruction.LField,
                    instruction.MField) <= 0) {
            printError(ERROR_WRITING_FILE_FAILED);
            
            return SIGNAL_FAILURE;
        }
        else {

            // Increase the tunnel's program counter for each instruction
            // printed to file.
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
        printError(ERROR_NULL_POINTER);

        return SIGNAL_FAILURE;
    }
  
    returnValue = SIGNAL_SUCCESS;

    // Emit each instruction, starting at the head of the queue. Delete
    // the nodes along the way.
    current = tunnel->queue->head;
    while (current != NULL) { 
        
        // If any call to emitInstruction fails, then change the returnValue to
        // failure. Note that this doesn't just return failure immediately, else
        // the rest of the queue would never get deleted, resulting in a memory leak.
        if (emitInstruction(tunnel, current->instruction, 0) == SIGNAL_FAILURE) {
            returnValue = SIGNAL_FAILURE;
        }

        current = current->next;
    }

    // Clear out the queue. I know this could have been done as the instructions
    // were emitted, but this is more organized and modular.
    clearInstructionQueue(tunnel->queue);

    return returnValue;
}

// Emit instructions for all the constants in the symbol table.
int setConstants(IOTunnel *tunnel, SymbolTable *table) {
    TableNode *current;
    Instruction instruction;

    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_POINTER);
        
        return SIGNAL_FAILURE;
    }
    
    current = table->head;
    while (current != NULL) {

        // If the current symbol is a constant, emit a LIT and STO instruction.
        if (current->symbol.type == LEX_CONST) {
            setInstruction(&instruction, LIT, 0, 0, current->symbol.value);
            if (emitInstruction(tunnel, instruction, 0) == SIGNAL_FAILURE) {
                return SIGNAL_FAILURE;
            }
            setInstruction(&instruction, STO, 0, 0, current->symbol.address);
            if (emitInstruction(tunnel, instruction, 0) == SIGNAL_FAILURE) {
                return SIGNAL_FAILURE;
            }
        }

        current = current->next;
    }

    return SIGNAL_SUCCESS;
}

// Return the tail of the instruction queue.
QueueNode *getQueueTail(IOTunnel *tunnel) {
    return (tunnel == NULL || tunnel->queue == NULL) ? NULL : tunnel->queue->tail;
}

// Load a token from the input stream.
int loadToken(IOTunnel *tunnel) {
    int i;
    char buffer;
    
    if (tunnel == NULL) {
        printError(ERROR_NULL_POINTER);
        
        return SIGNAL_FAILURE;
    }

    // If the input tunnel is done, then we've reached the end of the file unexpectedly.
    if (tunnel->status == SIGNAL_EOF) {
        printError(ERROR_UNEXPECTED_END_OF_FILE);
        tunnel->status = SIGNAL_FAILURE;
        
        return SIGNAL_FAILURE;
    }

    // Read the next token and the buffer behind it in the input file.
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

    // If we didn't reach the end of the file at some point in the function, set
    // the internal tunnel status to success, else it will remain as EOF. This
    // means that the next time this function is called the function will know
    // that the EOF has been reached before.
    if (tunnel->status != SIGNAL_EOF) {
        tunnel->status = SIGNAL_SUCCESS;
    }

    return SIGNAL_SUCCESS;
}

// Handle identifiers in the lexeme list.
int handleIdentifier(IOTunnel *tunnel) {
    char buffer;
    int i;

    if (tunnel == NULL) {
        printError(ERROR_NULL_POINTER);

        return SIGNAL_FAILURE;
    }

    // If the input file is through, we've got a problem.
    if (tunnel->status == SIGNAL_EOF) {
        printError(ERROR_IDENTIFIER_EXPECTED);
        
        return SIGNAL_FAILURE;
    }

    // Absorb an appropriate amount of characters.
    for (i = 0; i < IDENTIFIER_LEN; i++) {

        // If the file ends, adjust the tokenName.
        if (fscanf(tunnel->fin, "%c", tunnel->tokenName + i) == EOF) {
            tunnel->tokenName[i] = '\0';

            // If no tokens were added, then the tokenName is missing.
            if (i == 0) {
                printError(ERROR_IDENTIFIER_EXPECTED);
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
                    printError(ERROR_IDENTIFIER_EXPECTED);
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
        printError(ERROR_IDENTIFIER_TOO_LARGE, tunnel->tokenName);
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
        printError(ERROR_NULL_POINTER);

        return SIGNAL_FAILURE;
    }
    
    // If the input file is through, we've got a problem.
    if (tunnel->status == SIGNAL_EOF) {
        printError(ERROR_NUMBER_EXPECTED);
        
        return SIGNAL_FAILURE;
    }

    // If the expected number isn't there, throw a missing token error.
    if (fscanf(tunnel->fin, " %d", &(tunnel->tokenValue)) == EOF) {
        printError(ERROR_NUMBER_EXPECTED);
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
