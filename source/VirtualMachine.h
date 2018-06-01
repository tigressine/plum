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

#define MAX_LINES 100
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
    int registers[16];
    int stackPointer;
    int basePointer;
    int programCounter;
    int instructionCount;
    instruction instRegister;
} CPU;

CPU *createCPU(int);
void printCPU(CPU*);
int freeCPU(CPU*);
int countInstructions(char*);
instruction *loadInstructions(char*, int);
int processInstructions(instruction*, int);
int fetchInstruction(CPU*, instruction*);
int executeInstruction(CPU*);
int freeInstructions(instruction*);
#endif
