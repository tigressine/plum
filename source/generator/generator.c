// Part of Plum by Tiger Sachse.

#include "generator.h"

// Compile lexemes from the lexemeFile into usable bytecode.
int compileLexemes(char *lexemeFile, char *outFile, int options) {
    SymbolTable *table;
    IOTunnel *tunnel;
    int returnValue;

    // Create the symbol table.
    if ((table = createSymbolTable()) == NULL) {
        return SIGNAL_FAILURE;
    }
    
    // Create the input/output tunnel.
    if ((tunnel = createIOTunnel(lexemeFile, outFile)) == NULL) {
        return SIGNAL_FAILURE;
    }

    // Call the program class.
    returnValue = classProgram(tunnel, table); 

    // Print the symbol table, if requested.
    if (returnValue == SIGNAL_SUCCESS && checkOption(&options, OPTION_PRINT_SYMBOL_TABLE)) {
        printSymbolTable(table);
    }

    // Stay memory safe.
    destroyIOTunnel(tunnel);
    destroySymbolTable(table);

    return returnValue;
}
