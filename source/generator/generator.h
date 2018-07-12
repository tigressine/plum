// Part of Plum by Tiger Sachse.
#ifndef GENERATOR_H
#define GENERATOR_H

#include <limits.h>
#include "../plum.h"

// Different statuses possible for symbols in the symbol table.
enum Status {
    STATUS_ACTIVE,
    STATUS_INACTIVE
};

// Struct for symbols in a symbol table.
typedef struct Symbol {
    int type;
    int value;
    int level;
    int active;
    int address;
    char name[IDENTIFIER_LEN + 1];
} Symbol;

// Node that represents a column in a symbol table.
typedef struct TableNode {
    Symbol symbol;
    struct TableNode *next;
} TableNode;

// Symbol table container struct that implements a linked list.
typedef struct SymbolTable {
    int symbols;
    struct TableNode *head;
} SymbolTable;

// Table functional prototypes.
SymbolTable *createSymbolTable(void);
TableNode *createTableNode(int, int, int, int, int, char*, TableNode*);
int insertSymbol(SymbolTable*, int, int, int, int, int, char*);
Symbol *lookupSymbol(SymbolTable*, char*);
void destroySymbolTable(SymbolTable*);
int getTableSize(SymbolTable*);

#endif
