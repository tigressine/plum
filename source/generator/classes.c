// Part of Plum by Tiger Sachse

#include <string.h>
#include "generator.h"

// Syntactic class for the whole program.
// EBNF: classBlock ".".
int classProgram(IOTunnel *tunnel, SymbolTable *table) {
    Instruction instruction;

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

    // This class must end with a period. If everything works, then emit the
    // termination system call.
    if (tunnel->token != LEX_PERIOD) {
        printError(ERROR_SYMBOL_EXPECTED, '.');

        return SIGNAL_FAILURE;
    }
    else if (loadToken(tunnel) == SIGNAL_FAILURE || tunnel->status != SIGNAL_EOF) {
        printError(ERROR_TRAILING_CHARACTERS);

        return SIGNAL_FAILURE;
    }
    else {
        setInstruction(&instruction, SIO, 0, 0, 3);

        // Return whether the final emit call is successful.
        return emitInstruction(tunnel, instruction, 0);
    }
}

// Syntactic class for a block.
// EBNF: [subclassConstDeclaration][subclassVarDeclaration][classStatement].
int classBlock(IOTunnel *tunnel, SymbolTable *table) {
    char identifier[IDENTIFIER_LEN + 1];
    Instruction instruction;
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
   
    // Allocate the correct number of constants and variables on the stack.
    setInstruction(&instruction, INC, 0, 0, getTableSize(table) + INT_OFFSET);
    if (emitInstruction(tunnel, instruction, 0) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // Emit all the instructions to load constants into the stack.
    if (setConstants(tunnel, table) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // Handle all statements.
    return classStatement(tunnel, table, 0);
}

// Syntactic class for statements.
// EBNF: [subclassIdentifierStatement | subclassBeginStatement | subclassIfStatement |
//        subclassWhileStatement | subclassReadStatement | subclassWriteStatement].
int classStatement(IOTunnel *tunnel, SymbolTable *table, int nestedDepth) {
    Instruction instruction;
    Symbol *symbol;

    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    // Handle identifier statements.
    if (tunnel->token == LEX_IDENTIFIER) {

        // If the symbol is not in the symbol table, throw an error.
        if ((symbol = lookupSymbol(table, tunnel->tokenName)) == NULL) {
            printError(ERROR_UNDECLARED_IDENTIFIER);
            
            return SIGNAL_FAILURE;
        }
        
        // Call the identifier subclass.
        if (subclassIdentifierStatement(tunnel, table, nestedDepth) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        // Store the calculated value for the identifier into the stack with a STO
        // command. The calculated value will always be in register zero at this point.
        setInstruction(&instruction, STO, 0, 0, symbol->address);
        if (emitInstruction(tunnel, instruction, nestedDepth) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }

    // Handle begin/end multiline statements.
    else if (tunnel->token == LEX_BEGIN) {
        return subclassBeginStatement(tunnel, table, nestedDepth);
    }

    // Handle if statements.
    else if (tunnel->token == LEX_IF) {
        return subclassIfStatement(tunnel, table, nestedDepth); 
    }

    // Handle while statements.
    else if (tunnel->token == LEX_WHILE) {
        return subclassWhileStatement(tunnel, table, nestedDepth);
    }

    // Handle read statements.
    else if (tunnel->token == LEX_READ) {
        return subclassReadStatement(tunnel, table, nestedDepth);
    }

    // Handle write statements.
    else if (tunnel->token == LEX_WRITE) {
        return subclassWriteStatement(tunnel, table, nestedDepth);
    }

    // Else this is the empty string.
    else {
        return SIGNAL_SUCCESS;
    }
}

// Syntactic class for conditions.
// EBNF: "odd" classExpression | expression comparator expression.
int classCondition(IOTunnel *tunnel, SymbolTable *table, int nestedDepth) {
    Instruction instruction;
    
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    // Handle odd conditions.
    if (tunnel->token == LEX_ODD) {
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        if (classExpression(tunnel, table, 0, nestedDepth) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }

    // Handle all other conditions.
    else {

        // Get the left half of the condition.
        if (classExpression(tunnel, table, 0, nestedDepth) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
      
        // Current token must be one of these comparators.
        switch (tunnel->token) {
            case LEX_LESS:
                setInstruction(&instruction, LSS, 0, 0, 1);
                break;

            case LEX_EQUAL:
                setInstruction(&instruction, EQL, 0, 0, 1);
                break;

            case LEX_GREATER:
                setInstruction(&instruction, GTR, 0, 0, 1);
                break;

            case LEX_NOT_EQUAL:
                setInstruction(&instruction, NEQ, 0, 0, 1);
                break;

            case LEX_LESS_EQUAL:
                setInstruction(&instruction, LEQ, 0, 0, 1);
                break;

            case LEX_GREATER_EQUAL:
                setInstruction(&instruction, GEQ, 0, 0, 1);
                break;

            default:
                printError(ERROR_ILLEGAL_COMPARATOR, tunnel->token);

                return SIGNAL_FAILURE;
        }
        
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        // Get the right half of the condition.
        if (classExpression(tunnel, table, 1, nestedDepth) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        // Emit the appropriate comparison instruction, determined by the switch
        // case above.
        if (emitInstruction(tunnel, instruction, nestedDepth) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }
    
    return SIGNAL_SUCCESS;
}

// Syntactic class for expressions.
// EBNF: ["+" | "-"] classTerm {("+" | "-") classTerm}.
int classExpression(IOTunnel *tunnel, SymbolTable *table, int registerPosition, int nestedDepth) {
    Instruction instruction;
    int operation;

    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    // Prevent attempts to use more registers than available.
    if (registerPosition >= REGISTER_COUNT) {
        printError(ERROR_OUT_OF_REGISTERS);
        
        return SIGNAL_FAILURE;
    }

    // Accept positive or negative signs in front of terms.
    operation = tunnel->token;
    if (operation == LEX_PLUS || operation == LEX_MINUS) {
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }

    // Get the first term of the expression.
    if (classTerm(tunnel, table, registerPosition, nestedDepth) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // Negate the term if there was a minus sign.
    setInstruction(&instruction, NEG, registerPosition, registerPosition, 0);
    if (operation == LEX_MINUS) {
        if (emitInstruction(tunnel, instruction, nestedDepth) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }

    // Accept additional terms and save the operation at this point. It can be
    // overwritten in the following recursive calls so it must be saved here.
    operation = tunnel->token;
    while (operation == LEX_PLUS || operation == LEX_MINUS) {
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        // Find the next term and save in the register adjacent the current register.
        if (classTerm(tunnel, table, registerPosition + 1, nestedDepth) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
      
        // Set up the instruction to add/subtract the adjacent register to the current one.
        setInstruction(&instruction,
                       (operation == LEX_PLUS) ? ADD : SUB,
                       registerPosition,
                       registerPosition,
                       registerPosition + 1);
        if (emitInstruction(tunnel, instruction, nestedDepth) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        // Reset the operation.
        operation = tunnel->token;
    }

    return SIGNAL_SUCCESS;
}

// Syntactic class for terms.
// EBNF: classFactor {("*" | "/") classFactor}.
int classTerm(IOTunnel *tunnel, SymbolTable *table, int registerPosition, int nestedDepth) {
    Instruction instruction;
    int operation;

    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
   
    // Prevent attempts to use more registers than available.
    if (registerPosition >= REGISTER_COUNT) {
        printError(ERROR_OUT_OF_REGISTERS);
        
        return SIGNAL_FAILURE;
    }

    // Get the factor of this term.
    if (classFactor(tunnel, table, registerPosition, nestedDepth) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }
   
    // Absorb more factors.
    operation = tunnel->token;
    while (operation == LEX_MULTIPLY || operation == LEX_SLASH) {
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        // Get the next factor and save it into the next adjacent register.
        if (classFactor(tunnel, table, registerPosition + 1, nestedDepth) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        // Add the instruction to either multiply or divide the adjacent register
        // with the current one.
        setInstruction(&instruction,
                       (operation == LEX_MULTIPLY) ? MUL : DIV,
                       registerPosition,
                       registerPosition,
                       registerPosition + 1);
        if (emitInstruction(tunnel, instruction, nestedDepth) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        operation = tunnel->token;
    }

    return SIGNAL_SUCCESS;
}

// Syntactic class for factors.
// EBNF: identifier | number | "(" classExpression ")".
int classFactor(IOTunnel *tunnel, SymbolTable *table, int registerPosition, int nestedDepth) {
    Instruction instruction;
    Symbol *symbol;
    
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    // Prevent attempts to use more registers than available.
    if (registerPosition >= REGISTER_COUNT) {
        printError(ERROR_OUT_OF_REGISTERS);
        
        return SIGNAL_FAILURE;
    }

    // Factors can either be identifiers, numbers, or expressions.
    if (tunnel->token == LEX_IDENTIFIER) {

        // If the identifier is not in the symbol table, then it is undeclared.
        if ((symbol = lookupSymbol(table, tunnel->tokenName)) == NULL) {
            printError(ERROR_UNDECLARED_IDENTIFIER, tunnel->tokenName);
            
            return SIGNAL_FAILURE;
        }
       
        // Load the value of the identifier into the current register.
        setInstruction(&instruction, LOD, registerPosition, 0, symbol->address);
        if (emitInstruction(tunnel, instruction, nestedDepth) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
        
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }
    else if (tunnel->token == LEX_NUMBER) {

        // Load a literal into the current register.
        setInstruction(&instruction, LIT, registerPosition, 0, tunnel->tokenValue);
        if (emitInstruction(tunnel, instruction, nestedDepth) == SIGNAL_FAILURE) {
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

        // Call expression for the contents of the parenthetic expression.
        if (classExpression(tunnel, table, registerPosition, nestedDepth) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        // There must be a closing parenthesis.
        if (tunnel->token != LEX_RIGHT_PARENTHESIS) {
            printError(ERROR_SYMBOL_EXPECTED, ')');

            return SIGNAL_FAILURE;
        }
        
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
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
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        // Expect an identifier first.
        if (tunnel->token != LEX_IDENTIFIER) {
            printError(ERROR_IDENTIFIER_EXPECTED);

            return SIGNAL_FAILURE;
        }

        // Save the identifier's name for later.
        strcpy(identifier, tunnel->tokenName);

        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        // Next must be an equals sign.
        if (tunnel->token != LEX_EQUAL) {
            printError(ERROR_SYMBOL_EXPECTED, '=');

            return SIGNAL_FAILURE;
        }
        
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }

        // This final token is the numerical value of the constant.
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

    // The constant declarations line must terminate with a semicolon.
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

        // The first token must be an identifier.
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
int subclassIdentifierStatement(IOTunnel *tunnel, SymbolTable *table, int nestedDepth) {
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    if (loadToken(tunnel) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // Identifier statements must have a := symbol after the identifier.
    if (tunnel->token != LEX_BECOME) {
        printError(ERROR_BECOME_EXPECTED);

        return SIGNAL_FAILURE;
    }
    
    if (loadToken(tunnel) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // Find the resulting expression for the identifier.
    return classExpression(tunnel, table, 0, nestedDepth);
}

// Subclass for begin statements.
// EBNF: "begin" classStatement {";" classStatement} "end".
int subclassBeginStatement(IOTunnel *tunnel, SymbolTable *table, int nestedDepth) {
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    if (loadToken(tunnel) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // Begin multiline statements have at least one statement.
    if (classStatement(tunnel, table, nestedDepth) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // Absorb multiple statements in sequence.
    while (tunnel->token == LEX_SEMICOLON) {
        if (loadToken(tunnel) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
        
        if (classStatement(tunnel, table, nestedDepth) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }

    // Begin multiline statements must end with the word "end."
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
int subclassIfStatement(IOTunnel *tunnel, SymbolTable *table, int nestedDepth) {
    QueueNode *insertionPoint;
    Instruction instruction;
    int jumpTarget;

    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    if (loadToken(tunnel) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // Construct the condition for the if statement. This call increases the
    // nested depth by one (thus insuring that any resulting instructions are 
    // saved in the queue instead of printed to the file). Increasing the nestedDepth
    // is also critical for nested if and while statements.
    if (classCondition(tunnel, table, nestedDepth + 1) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // Get the insertion point in the instruction queue for the eventual JPC instruction.
    if ((insertionPoint = getQueueTail(tunnel)) == NULL) {
        return SIGNAL_FAILURE;
    }

    // The condition must be followed by the word "then."
    if (tunnel->token != LEX_THEN) {
        printError(ERROR_THEN_EXPECTED);

        return SIGNAL_FAILURE;
    }
    
    if (loadToken(tunnel) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }
  
    // Get the if statement's statements.
    if (classStatement(tunnel, table, nestedDepth + 1) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // Insert the JPC command at the insertionPoint in the instruction queue.
    jumpTarget = tunnel->programCounter + getQueueSize(tunnel->queue) + nestedDepth + 1;
    setInstruction(&instruction, JPC, 0, 0, jumpTarget);
    if (insertInstruction(tunnel->queue, instruction, insertionPoint) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }
   
    // If we aren't nested anymore, emit all of the instructions in the instruction queue.
    if (nestedDepth <= 0) {
        emitInstructions(tunnel);
    }

    return SIGNAL_SUCCESS;
}

// Subclass for while statements.
// EBNF: "while" classCondition "do" classStatement.
int subclassWhileStatement(IOTunnel *tunnel, SymbolTable *table, int nestedDepth) {
    QueueNode *insertionPoint;
    Instruction instruction;
    int returnTarget;
    int jumpTarget;

    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
    
    if (loadToken(tunnel) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // Save the return target for the JMP command that will be inserted at the
    // bottom of the while statement.
    returnTarget = tunnel->programCounter + getQueueSize(tunnel->queue) + nestedDepth;

    // Get the while statement's condition.
    if (classCondition(tunnel, table, nestedDepth + 1) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // Save the insertion point for the JPC command that controls the loop.
    if ((insertionPoint = getQueueTail(tunnel)) == NULL) {
        return SIGNAL_FAILURE;
    }

    // A "do" keyword is expected after the condition.
    if (tunnel->token != LEX_DO) {
        printError(ERROR_DO_EXPECTED);

        return SIGNAL_FAILURE;
    }

    if (loadToken(tunnel) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // Get the while statement's statements.
    if (classStatement(tunnel, table, nestedDepth + 1) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // Add the jump instruction at the bottom of all the statements to go
    // back to the top of the loop.
    setInstruction(&instruction, JMP, 0, 0, returnTarget);
    if (emitInstruction(tunnel, instruction, nestedDepth + 1) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // Insert the JPC command at the insertion point.
    jumpTarget = tunnel->programCounter + getQueueSize(tunnel->queue) + nestedDepth + 1;
    setInstruction(&instruction, JPC, 0, 0, jumpTarget);
    if (insertInstruction(tunnel->queue, instruction, insertionPoint) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }
    
    // If we are not nested, emit all the instructions in the instruction queue.
    if (nestedDepth <= 0) {
        emitInstructions(tunnel);
    }

    return SIGNAL_SUCCESS;
}

// Subclass for read statements.
// EBNF: "read" identifier.
int subclassReadStatement(IOTunnel *tunnel, SymbolTable *table, int nestedDepth) {
    Instruction instruction;
    Symbol *symbol;
    
    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
    
    if (loadToken(tunnel) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }
   
    // Can only read into identifiers.
    if (tunnel->token != LEX_IDENTIFIER) {
        printError(ERROR_IDENTIFIER_EXPECTED);
        
        return SIGNAL_FAILURE;
    }
  
    // If the symbol is not in the table, read cannot proceed.
    if ((symbol = lookupSymbol(table, tunnel->tokenName)) == NULL) {
        printError(ERROR_UNDECLARED_IDENTIFIER, tunnel->tokenName);

        return SIGNAL_FAILURE;
    }

    // Create the read system call.
    setInstruction(&instruction, SIO, 0, 0, 2);
    if (emitInstruction(tunnel, instruction, nestedDepth) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }
   
    // Store the read value into the appropriate place in the stack.
    setInstruction(&instruction, STO, 0, 0, symbol->address);
    if (emitInstruction(tunnel, instruction, nestedDepth) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    if (loadToken(tunnel) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    return SIGNAL_SUCCESS;
}

// Subclass for write statements.
// EBNF: "write" identifier.
int subclassWriteStatement(IOTunnel *tunnel, SymbolTable *table, int nestedDepth) {
    Instruction instruction;
    Symbol *symbol;

    if (tunnel == NULL || table == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
    
    if (loadToken(tunnel) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }

    // Only identifiers can be written.
    if (tunnel->token != LEX_IDENTIFIER) {
        printError(ERROR_IDENTIFIER_EXPECTED);
        
        return SIGNAL_FAILURE;
    }
   
    // If the symbol is not in the symbol table, throw an error.
    if ((symbol = lookupSymbol(table, tunnel->tokenName)) == NULL) {
        printError(ERROR_UNDECLARED_IDENTIFIER);
        
        return SIGNAL_FAILURE;
    }

    // Load the correct identifier into register zero.
    setInstruction(&instruction, LOD, 0, 0, symbol->address);
    if (emitInstruction(tunnel, instruction, nestedDepth) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }
    
    // Create the system call to print register zero.
    setInstruction(&instruction, SIO, 0, 0, 1);
    if (emitInstruction(tunnel, instruction, nestedDepth) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }
    
    if (loadToken(tunnel) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }
    
    return SIGNAL_SUCCESS;
}
