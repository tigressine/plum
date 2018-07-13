// Part of Plum by Tiger Sachse

#include "generator.h"

// Syntactic class for the whole program.
int classProgram(IOTunnel *tunnel, SymbolTable *table) {
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
  
    // Attempt to load the first token.
    loadToken(tunnel);
    if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
        return SIGNAL_FAILURE;
    }

    // Attempt to verify the inner block.
    if (classBlock(tunnel, table) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // This class must end with a period.
    if (tunnel->token != LEX_PERIOD) {
        printError(ERROR_SYMBOL_EXPECTED, '.');

        return SIGNAL_FAILURE;
    }
    else {
        loadToken(tunnel);
        if (tunnel->status != SIGNAL_EOF) {
            printError(ERROR_TRAILING_CHARACTERS);

            return SIGNAL_FAILURE;
        }

        return SIGNAL_SUCCESS;
    }
}

int classBlock(IOTunnel *tunnel, SymbolTable *table) {
    while (tunnel->token != LEX_PERIOD) {
        loadToken(tunnel);
        if (tunnel->status != SIGNAL_SUCCESS) {
            return tunnel->status;
        }
    }
    
    return SIGNAL_SUCCESS;
}

int classStatement(IOTunnel *tunnel, SymbolTable *table) {
    return SIGNAL_SUCCESS;
}

int classCondition(IOTunnel *tunnel, SymbolTable *table) {
    return SIGNAL_SUCCESS;
}

int classExpression(IOTunnel *tunnel, SymbolTable *table) {
    return SIGNAL_SUCCESS;
}

int classTerm(IOTunnel *tunnel, SymbolTable *table) {
    return SIGNAL_SUCCESS;
}

int classFactor(IOTunnel *tunnel, SymbolTable *table) {
    return SIGNAL_SUCCESS;
}
