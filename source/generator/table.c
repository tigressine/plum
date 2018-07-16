// Part of Plum by Tiger Sachse.

#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "generator.h"

// Create a new symbol table.
SymbolTable *createSymbolTable(void)  {
    SymbolTable *table;

    if ((table = calloc(1, sizeof(SymbolTable))) == NULL) {
        printError(ERROR_OUT_OF_MEMORY);
    }
   
    // This will be changed in the future. The offset was added to
    // allow printing some values that aren't necessary in this implementation
    // of a stack virtual machine.
    table->currentAddress = INT_OFFSET;

    return table;
}

// Create a column for a symbol table.
TableNode *createTableNode(int type,
                           int value,
                           int level,
                           int active,
                           int address,
                           char *name,
                           TableNode *next) {
    TableNode *new;

    // If the name is too long, return nothing.
    if (strlen(name) > IDENTIFIER_LEN) {
        printError(ERROR_IDENTIFIER_TOO_LARGE, name);
        
        return NULL;
    }

    // If the call to malloc fails, return nothing.
    if ((new = malloc(sizeof(TableNode))) == NULL) {
        printError(ERROR_OUT_OF_MEMORY);
        
        return NULL;
    }

    // Set all the members of the new node.
    new->next = next;
    new->symbol.type = type;
    new->symbol.value = value;
    new->symbol.level = level;
    new->symbol.active = active;
    new->symbol.address = address;
    strcpy(new->symbol.name, name);

    return new;
}

// Insert a new symbol column in the symbol table.
int insertSymbol(SymbolTable *table,
                 int type,
                 int value,
                 int level,
                 int active,
                 char *name) {
    TableNode *new;

    if (table == NULL) {
        printError(ERROR_NULL_POINTER);
        
        return SIGNAL_FAILURE;
    }

    // All entries in the symbol table must be unique.
    if (lookupSymbol(table, name) != NULL) {
        printError(ERROR_IDENTIFIER_ALREADY_DECLARED, name);

        return SIGNAL_FAILURE;
    }

    new = createTableNode(type, value, level, active, table->currentAddress, name, table->head);
    table->currentAddress++;

    // If a new node could not be created, return failure.
    if (new == NULL) {
        printError(ERROR_OUT_OF_MEMORY);

        return SIGNAL_FAILURE;
    }
    
    // Set the head of the table to be the new node. The table is saved in reverse
    // order using this scheme (like a stack). The old head is not forgotten
    // because it was set as new's next pointer in the call to createTableNode.
    else {
        table->head = new;
        table->symbols++;
    
        return SIGNAL_SUCCESS;
    }
}

// Hunt down a symbol in the table.
Symbol *lookupSymbol(SymbolTable *table, char *name) {
    TableNode *current;

    if (table == NULL) {
        printError(ERROR_NULL_POINTER);
        
        return NULL;
    }

    current = table->head;
    while (current != NULL) {
        
        // Skip inactive entries if necessary.
        if (current->symbol.active == STATUS_INACTIVE) {
            current = current->next;
        }
        
        // Return a pointer to the symbol if the name matches.
        else if (strcmp(current->symbol.name, name) == 0) {
            return &(current->symbol);
        }
        
        // Go to the next column in the table.
        else {
            current = current->next;
        }
    }

    // The symbol was not in the table, so return nothing.
    return NULL; 
}

// Get the size of the table.
int getTableSize(SymbolTable *table) {
    return (table == NULL) ? 0 : table->symbols;
}

// Free all memory associated with the table.
void destroySymbolTable(SymbolTable *table) {
    TableNode *next;
    TableNode *current;

    if (table == NULL) {
        printError(ERROR_NULL_POINTER);
        
        return;
    }

    // Delete all the nodes.
    current = table->head;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    free(table);
}
