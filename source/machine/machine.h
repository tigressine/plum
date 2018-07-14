// Part of Plum by Tiger Sachse.

#ifndef MACHINE_H
#define MACHINE_H

#include "../plum.h"

// Constants.
#define MAX_LINES 1000

// Instruction struct for each line of PL/0 code.
typedef struct Instruction {
    int opCode;
    int RField;
    int LField;
    int MField;
} Instruction;

// CPU struct to hold registers and the current instruction.
typedef struct CPU {
    int registers[REGISTER_COUNT];
    int programCounter;
    int instructionCount;
    Instruction instRegister;
} CPU;

// An item in an activation record stack. Also serves as a node in
// a linked list (that's how the stack is implemented).
typedef struct RecordStackItem {
    int *locals;
    int localCount;
    int returnValue;
    int returnAddress;
    struct RecordStackItem *staticLink;
    struct RecordStackItem *dynamicLink;
} RecordStackItem;

// Container struct for a record linked list struct. Also keeps track
// of the number of nodes in the stack.
typedef struct RecordStack {
    int records;
    RecordStackItem *currentRecord;
} RecordStack;

// Machine functional prototypes.
int startMachine(char*, int);
CPU *createCPU(int);
int destroyCPU(CPU*);
int countInstructions(char*);
Instruction *loadInstructions(char*, int);
int processInstructions(Instruction*, int, int);
int fetchInstruction(CPU*, Instruction*);
int executeInstruction(CPU*, RecordStack*);
int destroyInstructions(Instruction*);

// Operations functional prototypes.
int invalidRegister(int);
int invalidCPUState(CPU*, int);
int operationLiteral(CPU*);
int operationReturn(CPU*, RecordStack*);
int operationLoad(CPU*, RecordStack*);
int operationStore(CPU*, RecordStack*);
int operationCall(CPU*, RecordStack*);
int operationAllocate(CPU*, RecordStack*);
int operationJump(CPU*);
int operationConditionalJump(CPU*);
int operationSystemCall(CPU*);
int operationNegate(CPU*);
int operationAdd(CPU*);
int operationSubtract(CPU*);
int operationMultiply(CPU*);
int operationDivide(CPU*);
int operationIsOdd(CPU*);
int operationModulus(CPU*);
int operationIsEqual(CPU*);
int operationIsNotEqual(CPU*);
int operationIsLessThan(CPU*);
int operationIsLessThanOrEqualTo(CPU*);
int operationIsGreaterThan(CPU*);
int operationIsGreaterThanOrEqualTo(CPU*);

// Stack functional prototypes.
RecordStack *initializeRecordStack(void);
int pushRecord(CPU*, RecordStack*);
int popRecord(RecordStack*);
RecordStackItem *peekRecord(RecordStack*);
int allocateLocals(RecordStackItem*, int);
RecordStackItem *getDynamicParent(RecordStack*, int);
RecordStackItem *getStaticParent(RecordStack*, int);
int isEmpty(RecordStack*);
RecordStack *destroyRecordStack(RecordStack*);

// Printer functional prototypes.
void printStackTraceHeader(int);
void printStackTraceLine(CPU*, RecordStack*, int);
void printRegisters(CPU*);
void printCPU(CPU*);
void printRecords(RecordStackItem*);

#endif
