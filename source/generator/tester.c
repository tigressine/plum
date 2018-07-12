#include <stdio.h>
#include "generator.h"

void printTableRow(TableNode *current) {
    if (current == NULL) {
        return;
    }

    printTableRow(current->next);

    printf("%d, %d, %d, %d, %d, %s\n",
           current->symbol.type,
           current->symbol.value,
           current->symbol.level,
           current->symbol.active,
           current->symbol.address,
           current->symbol.name);
}

void printTable(SymbolTable *table) {
    if (table == NULL) {
        printf("Table does not exist.\n");

        return;
    } 

    printf("TYPE VALUE LEVEL ACTIVE ADDRESS NAME\n");
    printTableRow(table->head);
}

int _main(void) {
    SymbolTable *table;
    Symbol *symbol1;
    Symbol *symbol2;
    Symbol *symbol3;

    table = createSymbolTable();

    insertSymbol(table, 1, 100, 0, STATUS_ACTIVE, 5, "Germany");
    insertSymbol(table, 1, 10, 1, STATUS_ACTIVE, 6, "France");
    insertSymbol(table, 2, 1, 1, STATUS_INACTIVE, 7, "Britain");
    insertSymbol(table, 1, 200, 0, STATUS_ACTIVE, 8, "Portugal");

    insertSymbol(table, 1, 2, 3, 4, 5, "waytoooooooooooolongforsureohnowhathappes");
    insertSymbol(table, 1, 2, 3, 4, 5, "12345678901");
    insertSymbol(table, 1, 2, 3, 4, 5, "123456789012");
    insertSymbol(NULL, 1, 2, 3, 4, 5, "not here");
    printTable(table);
    printTable(NULL);


    symbol1 = lookupSymbol(table, "Germanyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy");
    symbol2 = lookupSymbol(table, "not in table");
    symbol3 = lookupSymbol(table, "Britain");

    if (symbol1 != NULL) {
        printf("symbol1 found %s\n", symbol1->name);
    }
    else {
        printf("failure\n");
    }
    if (symbol2 != NULL) {
        printf("symbol2 found %s\n", symbol2->name);
    }
    else {
        printf("failure\n");
    }
    if (symbol3 != NULL) {
        printf("symbol3 found %s\n", symbol3->name);
    }
    else {
        printf("failure\n");
    }

    printf("table size%d\n", getTableSize(table));
    printf("table size%d\n", getTableSize(NULL));

    destroySymbolTable(table);

    return 0;
}
