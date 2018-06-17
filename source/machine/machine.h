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
int opLiteral(CPU*);
int opReturn(CPU*, recordStack*);
int opLoad(CPU*, recordStack*);
int opStore(CPU*, recordStack*);
int opCall(CPU*, recordStack*);
int opAllocate(CPU*, recordStack*);
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
recordStackItem *getDynamicParent(recordStack*, int);
recordStackItem *getStaticParent(recordStack*, int);
int isEmpty(recordStack*);
recordStack *destroyRecordStack(recordStack*);
