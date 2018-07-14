// Part of Plum by Tiger Sachse

#include <string.h>
#include "generator.h"

// Syntactic class for the whole program.
// EBNF: classBlock ".".
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

    if (classBlock(tunnel, table) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // This class must end with a period.
    if (tunnel->token != LEX_PERIOD) {
        printError(ERROR_SYMBOL_EXPECTED, '.');

        return SIGNAL_FAILURE;
    }
    else {

        // If anything follows the period, there is an error.
        loadToken(tunnel);
        if (tunnel->status != SIGNAL_EOF) {
            printError(ERROR_TRAILING_CHARACTERS);

            return SIGNAL_FAILURE;
        }

        return SIGNAL_SUCCESS;
    }
}

// Syntactic class for a block.
// EBNF: [subclassConstDeclaration][subclassVarDeclaration][classStatement].
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

    // Handle all statements.
    return classStatement(tunnel, table);
}

// Syntactic class for statements.
// EBNF: [subclassIdentifierStatement | subclassBeginStatement | subclassIfStatement |
//        subclassWhileStatement | subclassReadStatement | subclassWriteStatement].
int classStatement(IOTunnel *tunnel, SymbolTable *table) {
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
    else if (tunnel->token == LEX_READ) {
        return subclassReadStatement(tunnel, table);
    }
    else if (tunnel->token == LEX_WRITE) {
        return subclassWriteStatement(tunnel, table);
    }
    else {
        return SIGNAL_SUCCESS;//might be wrong
    }
}

// Syntactic class for conditions.
// EBNF: "odd" classExpression | expression comparator expression.
int classCondition(IOTunnel *tunnel, SymbolTable *table) {
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
      
        // Token must be one of these comparators.
        switch (tunnel->token) {
            case LEX_LESS:
            case LEX_EQUAL:
            case LEX_GREATER:
            case LEX_NOT_EQUAL:
            case LEX_LESS_EQUAL:
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

// Syntactic class for expressions.
// EBNF: ["+" | "-"] classTerm {("+" | "-") classTerm}.
int classExpression(IOTunnel *tunnel, SymbolTable *table) {
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    // Accept positive or negative signs in front of terms.
    if (tunnel->token == LEX_PLUS || tunnel->token == LEX_MINUS) {
        loadToken(tunnel);
        if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
            return SIGNAL_FAILURE;
        }
    }

    if (classTerm(tunnel, table) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // Accept additional terms.
    while (tunnel->token == LEX_PLUS || tunnel->token == LEX_MINUS) {
        loadToken(tunnel);
        if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
            return SIGNAL_FAILURE;
        }

        if (classTerm(tunnel, table) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }

    return SIGNAL_SUCCESS;
}

// Syntactic class for terms.
// EBNF: classFactor {("*" | "/") classFactor}.
int classTerm(IOTunnel *tunnel, SymbolTable *table) {
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

    return SIGNAL_SUCCESS;
}

// Syntactic class for factors.
// EBNF: identifier | number | "(" classExpression ")".
int classFactor(IOTunnel *tunnel, SymbolTable *table) {
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    // Factors can either be identifiers, numbers, or expressions.
    if (tunnel->token == LEX_IDENTIFIER) {
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

    return SIGNAL_SUCCESS;
}

// Subclass for constant declarations.
// EBNF: "const" identifier "=" number {"," identifier "=" number} ";".
int subclassConstDeclaration(IOTunnel *tunnel, SymbolTable *table) {
    char identifier[IDENTIFIER_LEN + 1];
    int value;
    
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
  
    // Do loops are so ugly!
    do {
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

// Subclass for variable declarations.
// EBNF: "var" identifier {"," identifier} ";".
int subclassVarDeclaration(IOTunnel *tunnel, SymbolTable *table) {
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
   
    // SOOOO UGLY!
    do {
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

// Subclass for identifier statements.
// EBNF: identifier ":=" classExpression.
int subclassIdentifierStatement(IOTunnel *tunnel, SymbolTable *table) {
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

    return classExpression(tunnel, table);
}

// Subclass for begin statements.
// EBNF: "begin" classStatement {";" classStatement} "end".
int subclassBeginStatement(IOTunnel *tunnel, SymbolTable *table) {
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

    // Absorb multiple statements in sequence.
    while (tunnel->token == LEX_SEMICOLON) {
        loadToken(tunnel);
        if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
            return SIGNAL_FAILURE;
        }
        
        if (classStatement(tunnel, table) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }

    if (tunnel->token != LEX_END) {
        printError(ERROR_END_EXPECTED);

        return SIGNAL_FAILURE;
    }

    loadToken(tunnel);
    if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
        return SIGNAL_FAILURE;
    }
    
    return SIGNAL_SUCCESS;
}

// Subclass for if statements.
// EBNF: "if" classCondition "then" classStatement.
int subclassIfStatement(IOTunnel *tunnel, SymbolTable *table) {
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

// Subclass for while statements.
// EBNF: "while" classCondition "do" classStatement.
int subclassWhileStatement(IOTunnel *tunnel, SymbolTable *table) {
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

// Subclass for read statements.
// EBNF: "read" identifier.
int subclassReadStatement(IOTunnel *tunnel, SymbolTable *table) {
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
    
    loadToken(tunnel);
    if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
        return SIGNAL_FAILURE;
    }
    
    if (tunnel->token != LEX_IDENTIFIER) {
        printError(ERROR_IDENTIFIER_EXPECTED);
        
        return SIGNAL_FAILURE;
    }
    
    loadToken(tunnel);
    if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
        return SIGNAL_FAILURE;
    }

    return SIGNAL_SUCCESS;
}

// Subclass for write statements.
// EBNF: "write" identifier.
int subclassWriteStatement(IOTunnel *tunnel, SymbolTable *table) {
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
    
    loadToken(tunnel);
    if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
        return SIGNAL_FAILURE;
    }

    if (tunnel->token != LEX_IDENTIFIER) {
        printError(ERROR_IDENTIFIER_EXPECTED);
        
        return SIGNAL_FAILURE;
    }
    
    loadToken(tunnel);
    if (tunnel->status == SIGNAL_FAILURE || tunnel->status == SIGNAL_EOF) {
        return SIGNAL_FAILURE;
    }
    
    return SIGNAL_SUCCESS;
}
