#include "../plum.h"

// Constants.
#define INT_OFFSET 4
#define MAX_LINES 1000
#define REGISTER_COUNT 16

enum Opcodes {
    LIT = 1,
    RTN, LOD, STO,
    CAL, INC, JMP,
    JPC, SIO, NEG,
    ADD, SUB, MUL,
    DIV, ODD, MOD,
    EQL, NEQ, LSS,
    LEQ, GTR, GEQ
};

// Instruction struct for each line of PL/0 code.
typedef struct instruction {
    int opCode;
    int RField;
    int LField;
    int MField;
} instruction;

// CPU struct to hold registers and the current instruction.
typedef struct CPU {
    int registers[REGISTER_COUNT];
    int programCounter;
    int instructionCount;
    instruction instRegister;
} CPU;

// An item in an activation record stack. Also serves as a node in
// a linked list (that's how the stack is implemented).
typedef struct recordStackItem {
    int *locals;
    int localCount;
    int returnValue;
    int returnAddress;
    struct recordStackItem *staticLink;
    struct recordStackItem *dynamicLink;
} recordStackItem;

// Container struct for a record linked list struct. Also keeps track
// of the number of nodes in the stack.
typedef struct recordStack {
    int records;
    recordStackItem *currentRecord;
} recordStack;

// Machine functional prototypes.
int startMachine(char*, int);
CPU *createCPU(int);
int destroyCPU(CPU*);
int countInstructions(char*);
instruction *loadInstructions(char*, int);
int processInstructions(instruction*, int, int);
int fetchInstruction(CPU*, instruction*);
int executeInstruction(CPU*, recordStack*);
int destroyInstructions(instruction*);

// Operations functional prototypes.
int invalidRegisters(int, ...);
int operationLiteral(CPU*);
int operationReturn(CPU*, recordStack*);
int operationLoad(CPU*, recordStack*);
int operationStore(CPU*, recordStack*);
int operationCall(CPU*, recordStack*);
int operationAllocate(CPU*, recordStack*);
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
recordStack *initializeRecordStack(void);
int pushRecord(CPU*, recordStack*);
int popRecord(recordStack*);
recordStackItem *peekRecord(recordStack*);
int allocateLocals(recordStackItem*, int);
recordStackItem *getDynamicParent(recordStack*, int);
recordStackItem *getStaticParent(recordStack*, int);
int isEmpty(recordStack*);
recordStack *destroyRecordStack(recordStack*);
