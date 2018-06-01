#include <limits.h>

#ifndef VIRTUALMACHINE_HEADER
#define VIRTUALMACHINE_HEADER

// Convenience definitions for PL/0 opcode keywords.
#define LIT 1
#define RTN 2
#define LOD 3
#define STO 4
#define CAL 5
#define INC 6
#define JMP 7
#define JPC 8
#define SIO 9
#define NEG 10
#define ADD 11
#define SUB 12
#define MUL 13
#define DIV 14
#define ODD 15
#define MOD 16
#define EQL 17
#define NEQ 18
#define LSS 19
#define LEQ 20
#define GTR 21
#define GEQ 22

// Constants.
#define MAX_LINES 100
#define REGISTER_COUNT 16
#define OP_FAILURE INT_MIN
#define OP_SUCCESS INT_MIN + 1
#define KILL_PROGRAM INT_MIN + 2

// Instruction struct for each line of PL/0 code.
typedef struct instruction {
    int opCode;
    int RField;
    int LField;
    int MField;
} instruction;

// CPU struct to hold registers and the current instruction. Both the
// basePointer and stackPointer are uneccessary using the implemented
// activation stack method, but they are included for now for legacy purposes.
typedef struct CPU {
    int registers[REGISTER_COUNT];
    int stackPointer;
    int basePointer;
    int programCounter;
    int instructionCount;
    instruction instRegister;
} CPU;

// An item in an activation record stack. Also serves as a node in
// a linked list (that's how the stack is implemented). Many of these
// members are unneccesary except for the project specifications.
typedef struct recordStackItem {
    int *locals;
    int localCount;
    int returnAddress;
    int returnValue;
    struct recordStackItem *dynamicLink;
    struct recordStackItem *staticLink;
} recordStackItem;

// Container struct for a record linked list struct. Also keeps track
// of the number of nodes in the stack.
typedef struct recordStack {
    int records;
    recordStackItem *currentRecord;
} recordStack;

// Processing functional prototypes.
CPU *createCPU(int);
void printCPU(CPU*);
int freeCPU(CPU*);
int countInstructions(char*);
instruction *loadInstructions(char*, int);
int processInstructions(instruction*, int);
int fetchInstruction(CPU*, instruction*);
int executeInstruction(CPU*);
int freeInstructions(instruction*);
void printStackTraceLine(CPU*, recordStack*);
void printRecords(recordStackItem*);

// Operations functional prototypes.
int opLiteral(CPU*);
int opReturn(CPU*);
int opLoad(CPU*);
int opStore(CPU*);
int opCall(CPU*);
int opAllocate(CPU*);
int opJump(CPU*);
int opConditionalJump(CPU*);
int opSystemCall(CPU*);
int opNegate(CPU*);
int opAdd(CPU*);
int opSubtract(CPU*);
int opMultiply(CPU*);
int opDivide(CPU*);
int opIsOdd(CPU*);
int opModulus(CPU*);
int opIsEqual(CPU*);
int opIsNotEqual(CPU*);
int opIsLessThan(CPU*);
int opIsLessThanOrEqualTo(CPU*);
int opIsGreaterThan(CPU*);
int opIsGreaterThanOrEqualTo(CPU*);

// Stack functional prototypes.
recordStack *initializeRecordStack(void);
int pushRecord(CPU*, recordStack*);
int popRecord(recordStack*);
recordStackItem *peekRecord(recordStack*);
int allocateLocals(recordStackItem*, int);
//int storeValue(recordStack*, int, int, int);
recordStackItem *findRecord(recordStack*, int);
int isEmpty(recordStack*);
recordStack *destroyRecordStack(recordStack*);
#endif
