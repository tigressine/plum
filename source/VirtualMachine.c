#include <stdio.h>
#include <stdlib.h>
#include "VirtualMachine.h"
#include "Operations.c"

instruction *loadInstructions(char*, int);
int countInstructions(char*);
int fetchInstruction(CPU*, instruction*);
CPU *createCPU(int);
int freeCPU(CPU*);
int executeInstruction(CPU*);
int processInstructions(instruction*, int);
int freeInstructions(instruction*);

int main(int argsCount, char **argsVector) {
    int i;
    int instructionCount;
    instruction *instructions;

    if (argsCount < 2) {
        printf("Please pass a compiled PL/0 file as your first argument.\n");

        return 0;
    }
    else if ((instructionCount = countInstructions(argsVector[1])) == OP_FAILURE) {
        printf("File could not be read or the file is too long. Check your file.\n");

        return 0;
    }
    else if ((instructions = loadInstructions(argsVector[1], instructionCount)) == NULL) {
        printf("File could not be read, or program ran out of memory loading instructions.\n");
    
        return 0;
    }

    processInstructions(instructions, instructionCount);

    freeInstructions(instructions);

    return 0;
}

CPU *createCPU(int instructionCount) {
    CPU *cpu;

    if ((cpu = calloc(1, sizeof(CPU))) != NULL) {
        cpu->basePointer = 1;
        cpu->instructionCount = instructionCount;
    }

    return cpu;
}

int freeCPU(CPU *cpu) {
    free(cpu);

    return OP_SUCCESS; 
}

int fetchInstruction(CPU *cpu, instruction *instructions) {
    if (cpu == NULL || instructions == NULL) {
        return OP_FAILURE;
    }

    if (cpu->programCounter > 0 && cpu->programCounter < cpu->instructionCount) {
        cpu->instRegister = instructions[cpu->programCounter];
        cpu->programCounter++;
    }
    else {
        return OP_FAILURE;
    }

    return OP_SUCCESS;
}

int executeInstruction(CPU *cpu) {
    if (cpu == NULL) {
        return OP_FAILURE;
    }

    switch (cpu->instRegister.opCode) {
        case LIT: return opLiteral(cpu);
        case RTN: return opReturn(cpu);
        case LOD: return opLoad(cpu);
        case STO: return opStore(cpu);
        case CAL: return opCall(cpu);
        case INC: return opAllocate(cpu);
        case JMP: return opJump(cpu);
        case JPC: return opConditionalJump(cpu);
        case SIO: return opSystemCall(cpu);
        case NEG: return opNegate(cpu);
        case ADD: return opAdd(cpu);
        case SUB: return opSubtract(cpu);
        case MUL: return opMultiply(cpu);
        case DIV: return opDivide(cpu);
        case ODD: return opIsOdd(cpu);
        case MOD: return opModulus(cpu);
        case EQL: return opIsEqual(cpu);
        case NEQ: return opIsNotEqual(cpu);
        case LSS: return opIsLessThan(cpu);
        case LEQ: return opIsLessThanOrEqualTo(cpu);
        case GTR: return opIsGreaterThan(cpu);
        case GEQ: return opIsGreaterThanOrEqualTo(cpu);
        
        default:
            return OP_FAILURE;
    }
}

int processInstructions(instruction *instructions, int instructionCount) {
    int i;
    CPU *cpu;

    if ((cpu = createCPU(instructionCount)) == NULL) {
        return OP_FAILURE;
    }
   
    if ((fetchInstruction(cpu, instructions)) == OP_FAILURE) {
        freeCPU(cpu);

        return OP_FAILURE;
    }
   

    freeCPU(cpu);

    return OP_SUCCESS;
}

int countInstructions(char *path) {
    FILE *f;
    int count;
    char buffer;

    // If the file can't be opened, return OP_FAILURE.
    if ((f = fopen(path, "r")) == NULL) {
        return OP_FAILURE;
    }

    // Count the number of lines in the file.
    count = 0;
    while (fscanf(f, "%c", &buffer) != EOF) {
        if (buffer == '\n') {
            if (count < MAX_LINES) {
                count++;
            }
            // If the maximum number of lines has been reached, close the
            // file and return OP_FAILURE.
            else {
                fclose(f);

                return OP_FAILURE;
            }
        }
    }
    
    fclose(f);

    return count;
}

int freeInstructions(instruction *instructions) {
    free(instructions);

    return OP_SUCCESS;
}

// Load compiled instructions from a file at path.
instruction *loadInstructions(char *path, int instructionCount) {
    int i;
    FILE *f;
    instruction *instructions;

    // If the file can't be opened, return NULL.
    if ((f = fopen(path, "r")) == NULL) {
        return NULL;
    }

    // Create an array of instructions of length instructionCount. If the allocation fails,
    // close the file and return NULL.
    if ((instructions = malloc(sizeof(instruction) * instructionCount)) == NULL) {
        fclose(f);

        return NULL;
    }

    // For each line in the file, set the appropriate values for the corresponding
    // location in the new instructions array.
    for (i = 0; i < instructionCount; i++) {
        fscanf(f, "%d %d %d %d\n", &instructions[i].opCode,
                                   &instructions[i].RField,
                                   &instructions[i].LField,
                                   &instructions[i].MField);
    }

    fclose(f);

    return instructions;
}
