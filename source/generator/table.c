#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "generator.h"

// Create a new symbol table.
SymbolTable *createSymbolTable(void)  {
    return calloc(1, sizeof(SymbolTable));
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
        return NULL;
    }

    // If the call to malloc fails, return nothing.
    if ((new = malloc(sizeof(TableNode))) == NULL) {
        return NULL;
    }

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
                  int address,
                  char *name) {
    TableNode *new;

    if (table == NULL) {
        return SIGNAL_FAILURE;
    }

    new = createTableNode(type, value, level, active, address, name, table->head);
    
    // If a new node could not be created, return failure.
    if (new == NULL) {
        return SIGNAL_FAILURE;
    }
    else {
        // Set the head of the table to be the new node. The table is saved in reverse
        // order using this scheme (like a stack). The old head is not forgotten
        // because it was set as new's next pointer in the call to createTableNode.
        table->head = new;
        table->symbols++;
    
        return SIGNAL_SUCCESS;
    }
}

// Hunt down a symbol in the table.
Symbol *lookupSymbol(SymbolTable *table, char *name) {
    TableNode *current;

    if (table == NULL) {
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
    TableNode *current;
    TableNode *next;

    if (table == NULL) {
        return;
    }

    current = table->head;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    free(table);
}
