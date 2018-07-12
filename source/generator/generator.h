#ifndef GENERATOR_H
#define GENERATOR_H

#include <limits.h>
#include "../plum.h"

typedef struct Symbol {
    int type;
    int value;
    int level;
    int active;
    int address;
    char name[IDENTIFIER_LEN + 1];
} Symbol;

typedef struct TableNode {
    Symbol symbol;
    struct TableNode *next;
} TableNode;

typedef struct SymbolTable {
    int symbols;
    struct TableNode *head;
} SymbolTable;

// Table prototypes.
SymbolTable *createSymbolTable(void);
TableNode *createTableNode(int, int, int, int, int, char*, TableNode*);
int insertSymbol(SymbolTable*, int, int, int, int, int, char*);
Symbol *lookupSymbol(SymbolTable*, char*);
void destroySymbolTable(SymbolTable*);
int getTableSize(SymbolTable*);

#endif
