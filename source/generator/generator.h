// Part of Plum by Tiger Sachse.
#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>
#include <limits.h>
#include "../plum.h"

// Nodes for the instruction queue.
typedef struct QueueNode {
    Instruction instruction;
    struct QueueNode *next;
} QueueNode;

// A queue of instructions for implementing if blocks.
typedef struct InstructionQueue {
    QueueNode *head;
    QueueNode *tail;
    int length;
} InstructionQueue;

// A tunnel for input and output streams, as well as token storage.
typedef struct IOTunnel {
    int token;
    FILE *fin;
    FILE *fout;
    int status;
    int tokenValue;
    int programCounter;
    InstructionQueue *queue;
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
    int currentAddress;
    struct TableNode *head;
} SymbolTable;

// Generator functional prototypes.
void setInstruction(Instruction*, int, int, int, int);
int compileLexemes(char*, char*, int);

// Tunnel functional prototypes.
IOTunnel *createIOTunnel(char*, char*);
int emitInstruction(IOTunnel*, Instruction, int);
int emitInstructions(IOTunnel*);
int setConstants(IOTunnel*, SymbolTable*);
int loadToken(IOTunnel*);
int handleIdentifier(IOTunnel*);
int handleNumber(IOTunnel*);
void destroyIOTunnel(IOTunnel*);

// Table functional prototypes.
SymbolTable *createSymbolTable(void);
TableNode *createTableNode(int, int, int, int, int, char*, TableNode*);
int insertSymbol(SymbolTable*, int, int, int, int, char*);
Symbol *lookupSymbol(SymbolTable*, char*);
int getTableSize(SymbolTable*);
void destroySymbolTable(SymbolTable*);

// Class functional prototypes.
int classProgram(IOTunnel*, SymbolTable*);
int classBlock(IOTunnel*, SymbolTable*);
int classStatement(IOTunnel*, SymbolTable*);
int classCondition(IOTunnel*, SymbolTable*);
int classExpression(IOTunnel*, SymbolTable*, int);
int classTerm(IOTunnel*, SymbolTable*, int);
int classFactor(IOTunnel*, SymbolTable*, int);
int subclassConstDeclaration(IOTunnel*, SymbolTable*);
int subclassVarDeclaration(IOTunnel*, SymbolTable*);
int subclassIdentifierStatement(IOTunnel*, SymbolTable*);
int subclassBeginStatement(IOTunnel*, SymbolTable*);
int subclassIfStatement(IOTunnel*, SymbolTable*);
int subclassWhileStatement(IOTunnel*, SymbolTable*);
int subclassReadStatement(IOTunnel*, SymbolTable*);
int subclassWriteStatement(IOTunnel*, SymbolTable*);

// Printer functional prototypes.
void printSymbolTable(SymbolTable*);
void printSymbolTableColumn(TableNode*);

// Queue functional prototypes.
int isQueueEmpty(InstructionQueue*);
int getQueueSize(InstructionQueue*);
InstructionQueue *createInstructionQueue(void);
QueueNode *createQueueNode(Instruction);
int enqueueInstruction(InstructionQueue*, Instruction);
void clearInstructionQueue(InstructionQueue*);
void destroyInstructionQueue(InstructionQueue*);

#endif
