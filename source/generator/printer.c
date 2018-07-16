// Part of Plum by Tiger Sachse

#include <stdio.h>
#include "generator.h"

// Print the symbol table.
void printSymbolTable(SymbolTable *table) {
    if (table == NULL) {
        printf("No table.\n");
        
        return;
    }
   
    printf("Symbol Table:\n");
    printf("TYPE VALUE LEVEL ACTIVE ADDRESS NAME\n");
    printf("------------------------------------\n");
   
    // Recursively call print on the head of the table.
    printSymbolTableColumn(table->head);
    printf("\n");
}

// Print a column in the symbol table.
void printSymbolTableColumn(TableNode *node) {
    if (node == NULL) {
        return;
    }

    // Print the next column, then print this one. The table is saved
    // in reverse order so printing like this will reverse the reversed list.
    printSymbolTableColumn(node->next);
    printf("%-4d %-5d %-5d %-6d %-7d %s\n",
           node->symbol.type,
           node->symbol.value,
           node->symbol.level,
           node->symbol.active,
           node->symbol.address,
           node->symbol.name);
}
