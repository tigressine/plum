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
    if (loadToken(tunnel) == SIGNAL_FAILURE) {
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
    else if (loadToken(tunnel) == SIGNAL_FAILURE || tunnel->status != SIGNAL_EOF) {
        printError(ERROR_TRAILING_CHARACTERS);

        return SIGNAL_FAILURE;
    }
    else {
        
        // Return whether the final emit call is successful.
        return emitInstruction(tunnel, SIO, 0, 0, 3);
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

    if (emitInstruction(tunnel, INC, 0, 0, getTableSize(table) + INT_OFFSET) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    if (setConstants(tunnel, table) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // Handle all statements.
    return classStatement(tunnel, table);
}

// Syntactic class for statements.
// EBNF: [subclassIdentifierStatement | subclassBeginStatement | subclassIfStatement |
//        subclassWhileStatement | subclassReadStatement | subclassWriteStatement].
int classStatement(IOTunnel *tunnel, SymbolTable *table) {
    Symbol *symbol;

    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    if (tunnel->token == LEX_IDENTIFIER) {
        if ((symbol = lookupSymbol(table, tunnel->tokenName)) == NULL) {
            //printError(ERROR this identifer undeclared);
            
            return SIGNAL_FAILURE;
        }
        
        if (subclassIdentifierStatement(tunnel, table) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        if (emitInstruction(tunnel, STO, 0, 0, symbol->address) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
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
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        if (classExpression(tunnel, table, 0) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }
    else {
        if (classExpression(tunnel, table, 0) == SIGNAL_FAILURE) {
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
                if (loadToken(tunnel) == SIGNAL_FAILURE) {
                    return SIGNAL_FAILURE;
                }

                if (classExpression(tunnel, table, 0) == SIGNAL_FAILURE) {
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
int classExpression(IOTunnel *tunnel, SymbolTable *table, int registerPosition) {
    int operation;

    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    if (registerPosition >= REGISTER_COUNT) {
        //printError(out of registers);
        
        return SIGNAL_FAILURE;
    }

    // Accept positive or negative signs in front of terms.
    operation = tunnel->token;
    if (operation == LEX_PLUS || operation == LEX_MINUS) {
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }

    if (classTerm(tunnel, table, registerPosition) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // Negate the term if there was a minus sign.
    if (operation == LEX_MINUS) {
        if (emitInstruction(tunnel,
                            NEG,
                            registerPosition,
                            registerPosition, 0) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }

    //registerPosition++;

    // Accept additional terms.
    operation = tunnel->token;
    while (operation == LEX_PLUS || operation == LEX_MINUS) {
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        if (classTerm(tunnel, table, registerPosition + 1) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
        
        if (emitInstruction(tunnel,
                            (operation == LEX_PLUS) ? ADD : SUB,
                            registerPosition,
                            registerPosition,
                            registerPosition + 1) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        operation = tunnel->token;
    }

    return SIGNAL_SUCCESS;
}

// Syntactic class for terms.
// EBNF: classFactor {("*" | "/") classFactor}.
int classTerm(IOTunnel *tunnel, SymbolTable *table, int registerPosition) {
    int operation;

    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
   
    if (registerPosition >= REGISTER_COUNT) {
        //printError(out of registers);
        
        return SIGNAL_FAILURE;
    }

    if (classFactor(tunnel, table, registerPosition) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }
    
    //registerPosition++;

    operation = tunnel->token;
    while (operation == LEX_MULTIPLY || operation == LEX_SLASH) {
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        if (classFactor(tunnel, table, registerPosition + 1) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        if (emitInstruction(tunnel,
                            (operation == LEX_MULTIPLY) ? MUL : DIV,
                            registerPosition,
                            registerPosition,
                            registerPosition + 1) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        operation = tunnel->token;
    }

    return SIGNAL_SUCCESS;
}

// Syntactic class for factors.
// EBNF: identifier | number | "(" classExpression ")".
int classFactor(IOTunnel *tunnel, SymbolTable *table, int registerPosition) {
    Symbol *symbol;
    
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    if (registerPosition >= REGISTER_COUNT) {
        //printError(out of registers);
        
        return SIGNAL_FAILURE;
    }

    // Factors can either be identifiers, numbers, or expressions.
    if (tunnel->token == LEX_IDENTIFIER) {
        if ((symbol = lookupSymbol(table, tunnel->tokenName)) == NULL) {
            //printError(ERROR this identifer undeclared);
            
            return SIGNAL_FAILURE;
        }
        
        if (emitInstruction(tunnel,
                            LOD,
                            registerPosition, 0,
                            symbol->address) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
        
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }
    else if (tunnel->token == LEX_NUMBER) {
        if (emitInstruction(tunnel,
                            LIT,
                            registerPosition, 0,
                            tunnel->tokenValue) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
        
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }
    else if (tunnel->token == LEX_LEFT_PARENTHESIS) {
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        if (classExpression(tunnel, table, registerPosition) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        if (tunnel->token != LEX_RIGHT_PARENTHESIS) {
            printError(ERROR_SYMBOL_EXPECTED, ')');

            return SIGNAL_FAILURE;
        }
        
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }
    else {
        printError(ERROR_INVALID_FACTOR);//

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
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        if (tunnel->token != LEX_IDENTIFIER) {
            printError(ERROR_IDENTIFIER_EXPECTED);

            return SIGNAL_FAILURE;
        }

        // Save the identifier's name for later.
        strcpy(identifier, tunnel->tokenName);

        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        if (tunnel->token != LEX_EQUAL) {
            printError(ERROR_SYMBOL_EXPECTED, '=');

            return SIGNAL_FAILURE;
        }
        
        // This final token is the numerical value of the constant.
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
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
                         STATUS_ACTIVE,
                         identifier) == SIGNAL_FAILURE) {
            
            return SIGNAL_FAILURE;
        }

        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }
    while (tunnel->token == LEX_COMMA);

    if (tunnel->token != LEX_SEMICOLON) {
        printError(ERROR_SYMBOL_EXPECTED, ';');

        return SIGNAL_FAILURE;
    }
    
    if (loadToken(tunnel) == SIGNAL_FAILURE) {
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
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        if (tunnel->token != LEX_IDENTIFIER) {
            printError(ERROR_IDENTIFIER_EXPECTED);

            return SIGNAL_FAILURE;
        }

        // Attempt to insert the variable into the table.
        if (insertSymbol(table, LEX_VAR, 0, 0,
                         STATUS_ACTIVE, tunnel->tokenName) == SIGNAL_FAILURE) {
            
            return SIGNAL_FAILURE;
        }
        
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }
    while (tunnel->token == LEX_COMMA);

    // If the final token isn't a semicolon, we've got a problem.
    if (tunnel->token != LEX_SEMICOLON) {
        printError(ERROR_SYMBOL_EXPECTED, ';');

        return SIGNAL_FAILURE;
    }
    
    if (loadToken(tunnel) == SIGNAL_FAILURE) {
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

    if (loadToken(tunnel) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    if (tunnel->token != LEX_BECOME) {
        printError(ERROR_BECOME_EXPECTED);

        return SIGNAL_FAILURE;
    }
    
    if (loadToken(tunnel) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    return classExpression(tunnel, table, 0);
}

// Subclass for begin statements.
// EBNF: "begin" classStatement {";" classStatement} "end".
int subclassBeginStatement(IOTunnel *tunnel, SymbolTable *table) {
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    if (loadToken(tunnel) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    if (classStatement(tunnel, table) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // Absorb multiple statements in sequence.
    while (tunnel->token == LEX_SEMICOLON) {
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
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

    if (loadToken(tunnel) == SIGNAL_FAILURE) {
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

    if (loadToken(tunnel) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    if (classCondition(tunnel, table) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    if (tunnel->token != LEX_THEN) {
        printError(ERROR_THEN_EXPECTED);

        return SIGNAL_FAILURE;
    }
    
    if (loadToken(tunnel) == SIGNAL_FAILURE) {
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
    
    if (loadToken(tunnel) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    if (classCondition(tunnel, table) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    if (tunnel->token != LEX_DO) {
        printError(ERROR_DO_EXPECTED);

        return SIGNAL_FAILURE;
    }

    if (loadToken(tunnel) == SIGNAL_FAILURE) {
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
    
    if (loadToken(tunnel) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }
    
    if (tunnel->token != LEX_IDENTIFIER) {
        printError(ERROR_IDENTIFIER_EXPECTED);
        
        return SIGNAL_FAILURE;
    }
    
    if (loadToken(tunnel) == SIGNAL_FAILURE) {
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
    
    if (loadToken(tunnel) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    if (tunnel->token != LEX_IDENTIFIER) {
        printError(ERROR_IDENTIFIER_EXPECTED);
        
        return SIGNAL_FAILURE;
    }
    
    if (loadToken(tunnel) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }
    
    return SIGNAL_SUCCESS;
}
