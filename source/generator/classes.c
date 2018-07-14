// Part of Plum by Tiger Sachse

#include <string.h>
#include "generator.h"
//END
// Syntactic class for the whole program.
int classProgram(IOTunnel *tunnel, SymbolTable *table) {
    printf("program\n");
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
    printf("block\n");
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

    return classStatement(tunnel, table);
}

int subclassIdentifierStatement(IOTunnel *tunnel, SymbolTable *table) {
    printf("identifier\n");
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
    
    loadToken(tunnel);
    if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
        return SIGNAL_FAILURE;
    }

    if (tunnel->token != LEX_BECOME) {
        printError(ERROR_BECOME_EXPECTED);

        return SIGNAL_FAILURE;
    }
    
    loadToken(tunnel);
    if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
        return SIGNAL_FAILURE;
    }

    printf("done4]\n");
    return classExpression(tunnel, table);
}

int subclassBeginStatement(IOTunnel *tunnel, SymbolTable *table) {
    printf("begin\n");
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    loadToken(tunnel);
    if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
        return SIGNAL_FAILURE;
    }

    if (classStatement(tunnel, table) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    printf("done6\n");
    while (tunnel->token == LEX_SEMICOLON) {
        printf("here?\n");
        loadToken(tunnel);
        if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
            return SIGNAL_FAILURE;
        }
        
        if (classStatement(tunnel, table) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }

    if (tunnel->token != LEX_END) {
        printf("found\n");
        printError(ERROR_END_EXPECTED);

        return SIGNAL_FAILURE;
    }
    else {
        printf("end found!!!\n");
    }

    printf("done7\n");
    loadToken(tunnel);
    if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
        return SIGNAL_FAILURE;
    }
    printf("done8\n");
    return SIGNAL_SUCCESS;
}

int subclassIfStatement(IOTunnel *tunnel, SymbolTable *table) {
    printf("if\n");
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    loadToken(tunnel);
    if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
        return SIGNAL_FAILURE;
    }

    if (classCondition(tunnel, table) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    /*
    loadToken(tunnel);
    if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
        return SIGNAL_FAILURE;
    }*/

    if (tunnel->token != LEX_THEN) {
        printError(ERROR_THEN_EXPECTED);

        return SIGNAL_FAILURE;
    }
    
    loadToken(tunnel);
    if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
        return SIGNAL_FAILURE;
    }
    
    if (classStatement(tunnel, table) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    return SIGNAL_SUCCESS;
}

int subclassWhileStatement(IOTunnel *tunnel, SymbolTable *table) {
    printf("while\n");
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
    
    loadToken(tunnel);
    if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
        return SIGNAL_FAILURE;
    }

    if (classCondition(tunnel, table) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    if (tunnel->token != LEX_DO) {
        printError(ERROR_DO_EXPECTED);

        return SIGNAL_FAILURE;
    }

    loadToken(tunnel);
    if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
        return SIGNAL_FAILURE;
    }

    if (classStatement(tunnel, table) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    return SIGNAL_SUCCESS;
}

int classStatement(IOTunnel *tunnel, SymbolTable *table) {
    printf("statement\n");
    printf("token%d\n", tunnel->token);
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    if (tunnel->token == LEX_IDENTIFIER) {
        return subclassIdentifierStatement(tunnel, table);
    }
    else if (tunnel->token == LEX_BEGIN) {
        return subclassBeginStatement(tunnel, table);
    }
    else if (tunnel->token == LEX_IF) {
        return subclassIfStatement(tunnel, table); 
    }
    else if (tunnel->token == LEX_WHILE) {
        return subclassWhileStatement(tunnel, table);
    }
    else {
        printf("this]n\n");
        return SIGNAL_SUCCESS;//might be wrong
    }
}

int classCondition(IOTunnel *tunnel, SymbolTable *table) {
    printf("condition\n");
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    if (tunnel->token == LEX_ODD) {
        loadToken(tunnel);
        if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
            return SIGNAL_FAILURE;
        }

        if (classExpression(tunnel, table) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }
    else {
        if (classExpression(tunnel, table) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
       
        switch (tunnel->token) {
            case LEX_EQUAL:
            case LEX_LESS:
            //case LEX_WEIRD_ONE:
            case LEX_LESS_EQUAL:
            case LEX_GREATER:
            case LEX_GREATER_EQUAL:
                loadToken(tunnel);
                if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
                    return SIGNAL_FAILURE;
                }

                if (classExpression(tunnel, table) == SIGNAL_FAILURE) {
                    return SIGNAL_FAILURE;
                }
           
                break;
            
            default:
                printError(ERROR_NO_RELATIONAL_TOKEN);

                return SIGNAL_FAILURE;
        }
    }
    
    return SIGNAL_SUCCESS;
}

int classExpression(IOTunnel *tunnel, SymbolTable *table) {
    printf("expression\n");
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    if (tunnel->token == LEX_PLUS || tunnel->token == LEX_MINUS) {
        loadToken(tunnel);
        if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
            return SIGNAL_FAILURE;
        }
    }

    if (classTerm(tunnel, table) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    while (tunnel->token == LEX_PLUS || tunnel->token == LEX_MINUS) {
        loadToken(tunnel);
        if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
            return SIGNAL_FAILURE;
        }

        if (classTerm(tunnel, table) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }

    printf("done3\n");
    return SIGNAL_SUCCESS;
}

int classTerm(IOTunnel *tunnel, SymbolTable *table) {
    printf("term\n");
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
    
    if (classFactor(tunnel, table) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }
    
    while (tunnel->token == LEX_MULTIPLY || tunnel->token == LEX_SLASH) {
        loadToken(tunnel);
        if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
            return SIGNAL_FAILURE;
        }

        if (classFactor(tunnel, table) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }

    printf("done2\n");
    return SIGNAL_SUCCESS;
}

int classFactor(IOTunnel *tunnel, SymbolTable *table) {
    printf("factor\n");
    printf("tun%d\n", tunnel->token);
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    if (tunnel->token == LEX_IDENTIFIER) {
        printf("this is the extra\n");
        loadToken(tunnel);
        if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
            return SIGNAL_FAILURE;
        }
    }
    else if (tunnel->token == LEX_NUMBER) {
        loadToken(tunnel);
        if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
            return SIGNAL_FAILURE;
        }
    }
    else if (tunnel->token == LEX_LEFT_PARENTHESIS) {
        loadToken(tunnel);
        if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
            return SIGNAL_FAILURE;
        }

        if (classExpression(tunnel, table) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        if (tunnel->token != LEX_RIGHT_PARENTHESIS) {
            printError(ERROR_SYMBOL_EXPECTED, ')');

            return SIGNAL_FAILURE;
        }
        
        loadToken(tunnel);
        if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
            return SIGNAL_FAILURE;
        }
    }
    else {
        printError(ERROR_INVALID_FACTOR);

        return SIGNAL_FAILURE;
    }

    printf("done\n");
    return SIGNAL_SUCCESS;
}

// Add all constants to the symbol table.
int subclassConstDeclaration(IOTunnel *tunnel, SymbolTable *table) {
    printf("const dec\n");
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
    printf("var dec\n");
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
        printError(ERROR_SYMBOL_EXPECTED, ';');

        return SIGNAL_FAILURE;
    }
    
    loadToken(tunnel);
    if (tunnel->status == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    return SIGNAL_SUCCESS;
}
