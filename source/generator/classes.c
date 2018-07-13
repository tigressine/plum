// Part of Plum by Tiger Sachse

#include <string.h>
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

// Syntactic class for a block.
int classBlock(IOTunnel *tunnel, SymbolTable *table) {
    char identifier[IDENTIFIER_LEN + 1];
    int value;
    
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    // Handle constant declarations.
    if (tunnel->token == LEX_CONST) {
        if (subclassConstDeclaration(tunnel, table) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }

    // Handle variable declarations.
    if (tunnel->token == LEX_VAR) {
        if (subclassVarDeclaration(tunnel, table) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }

    return SIGNAL_SUCCESS;
}

int classStatement(IOTunnel *tunnel, SymbolTable *table) {
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
    return SIGNAL_SUCCESS;
}

int classCondition(IOTunnel *tunnel, SymbolTable *table) {
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
    return SIGNAL_SUCCESS;
}

int classExpression(IOTunnel *tunnel, SymbolTable *table) {
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
    return SIGNAL_SUCCESS;
}

int classTerm(IOTunnel *tunnel, SymbolTable *table) {
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
    return SIGNAL_SUCCESS;
}

int classFactor(IOTunnel *tunnel, SymbolTable *table) {
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
    return SIGNAL_SUCCESS;
}

// Add all constants to the symbol table.
int subclassConstDeclaration(IOTunnel *tunnel, SymbolTable *table) {
    char identifier[IDENTIFIER_LEN + 1];
    int value;
    
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
   
    do {

        // This first token should be the identifier.
        loadToken(tunnel);
        if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
            return SIGNAL_FAILURE;
        }

        if (tunnel->token != LEX_IDENTIFIER) {
            printError(ERROR_IDENTIFIER_EXPECTED);

            return SIGNAL_FAILURE;
        }

        // Save the identifier's name for later.
        strcpy(identifier, tunnel->tokenName);

        // This next token must be an equal sign.
        loadToken(tunnel);
        if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
            return SIGNAL_FAILURE;
        }

        if (tunnel->token != LEX_EQUAL) {
            printError(ERROR_SYMBOL_EXPECTED, '=');

            return SIGNAL_FAILURE;
        }
        
        // This final token is the numerical value of the constant.
        loadToken(tunnel);
        if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
            return SIGNAL_FAILURE;
        }

        if (tunnel->token != LEX_NUMBER) {
            printError(ERROR_NUMBER_EXPECTED);

            return SIGNAL_FAILURE;
        }
    
        // Put the constant into the symbol table.
        if (insertSymbol(table,
                         LEX_CONST,
                         tunnel->tokenValue, 0,
                         STATUS_ACTIVE, 0,
                         identifier) == SIGNAL_FAILURE) {
            
            return SIGNAL_FAILURE;
        }

        loadToken(tunnel);
        if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
            return SIGNAL_FAILURE;
        }
    }
    while (tunnel->token == LEX_COMMA);

    // After all the constants are declared, there must be a semicolon.
    if (tunnel->token != LEX_SEMICOLON) {
        printError(ERROR_SYMBOL_EXPECTED, ';');

        return SIGNAL_FAILURE;
    }
    
    loadToken(tunnel);
    if (tunnel->status == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    return SIGNAL_SUCCESS;
}

// Add all variables to the symbol table.
int subclassVarDeclaration(IOTunnel *tunnel, SymbolTable *table) {
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
    
    do {
        
        // This first token must be the variable identifier.
        loadToken(tunnel);
        if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
            return SIGNAL_FAILURE;
        }

        if (tunnel->token != LEX_IDENTIFIER) {
            printError(ERROR_IDENTIFIER_EXPECTED);

            return SIGNAL_FAILURE;
        }

        // Attempt to insert the variable into the table.
        if (insertSymbol(table, LEX_VAR, 0, 0,
                         STATUS_ACTIVE, 0, tunnel->tokenName) == SIGNAL_FAILURE) {
            
            return SIGNAL_FAILURE;
        }
        
        // The next token will be either a comma or a semicolon.
        loadToken(tunnel);
        if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
            return SIGNAL_FAILURE;
        }
    }
    while (tunnel->token == LEX_COMMA);

    // If the final token isn't a semicolon, we've got a problem.
    if (tunnel->token != LEX_SEMICOLON) {
        printf("%d\n", tunnel->token);
        printError(ERROR_SYMBOL_EXPECTED, ';');

        return SIGNAL_FAILURE;
    }
    
    loadToken(tunnel);
    if (tunnel->status == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    return SIGNAL_SUCCESS;
}
