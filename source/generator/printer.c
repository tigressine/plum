// Part of Plum by Tiger Sachse

#include <stdio.h>
#include "generator.h"

void printSymbolTable(SymbolTable *table) {
    if (table == NULL) {
        printf("No table.\n");
        
        return;
    }
   
    printf("Symbol Table:\n");
    printf("TYPE VALUE LEVEL ACTIVE ADDRESS NAME\n");
    printf("------------------------------------\n");
    printSymbolTableColumn(table->head);
}

void printSymbolTableColumn(TableNode *node) {
    if (node == NULL) {
        return;
    }

    printSymbolTableColumn(node->next);
    printf("%-4d %-5d %-5d %-6d %-7d %s\n",
           node->symbol.type,
           node->symbol.value,
           node->symbol.level,
           node->symbol.active,
           node->symbol.address,
           node->symbol.name);
}
