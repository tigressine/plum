// Part of Plum by Tiger Sachse.

#include "generator.h"

void setInstruction(Instruction *instruction, int opCode, int RField, int LField, int MField) {
    if (instruction == NULL) {
        return;
    }

    instruction->opCode = opCode;
    instruction->RField = RField;
    instruction->LField = LField;
    instruction->MField = MField;
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

    printSymbolTable(table);

    destroyIOTunnel(tunnel);
    destroySymbolTable(table);

    return returnValue;
}
