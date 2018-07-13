// Part of Plum by Tiger Sachse.
#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>
#include <limits.h>
#include "../plum.h"

// A tunnel for input and output streams, as well as token storage.
typedef struct IOTunnel {
    int token;
    FILE *fin;
    FILE *fout;
    int tokenValue;
    char tokenName[IDENTIFIER_LEN + 1];
} IOTunnel;

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

// Generator functional prototypes.
IOTunnel *createIOTunnel(char*, char*);
int loadToken(IOTunnel*);
int handleIdentifier(IOTunnel*);
int handleNumber(IOTunnel*);
void destroyIOTunnel(IOTunnel*);
int compileLexemes(char*, char*, int);

// Table functional prototypes.
SymbolTable *createSymbolTable(void);
TableNode *createTableNode(int, int, int, int, int, char*, TableNode*);
int insertSymbol(SymbolTable*, int, int, int, int, int, char*);
Symbol *lookupSymbol(SymbolTable*, char*);
void destroySymbolTable(SymbolTable*);
int getTableSize(SymbolTable*);

// Class functional prototypes.
int classProgram();
int classBlock();
int classStatement();
int classCondition();
int classExpression();
int classTerm();
int classFactor();

#endif
