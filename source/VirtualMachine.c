#include <stdio.h>
#include <stdlib.h>
#include "VirtualMachine.h"
#include "Operations.c"

instruction *loadInstructions(char*);

int main(int argsCount, char **argsVector) {
    instructionContainer *container;
    instruction *instructions;
    int instructionCount;
    int i;

    if (argsCount < 2) {
        printf("Please pass a compiled PL/0 file as your first argument.\n");

        return 0;//failure
    }
    else if ((instructions = loadInstructions(argsVector[1])) == NULL) {
        printf("Instructions could not be read. Exiting program.\n");
    
        return 0;//failure
    }

    return 0;
}

CPU *createCPU(void) {
    return calloc(1, sizeof(CPU));
}

int fetchInstruction(CPU *cpu, instruction *instructions) {
    if (cpu == NULL || instructions == NULL) {
        return OP_FAILURE;
    }

    if (cpu->programCounter > 0 && cpu->programCounter < )
    cpu->instRegister = instructions[cpu->programCounter];
    cpu->programCounter++;

    return OP_SUCCESS;// add check of program counter that it's in bounds?
}

int executeInstruction(CPU *cpu) {
    if (cpu == NULL) {
        return OP_FAILURE;//failure
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
            return OP_FAILURE;//failure
    }
}

int processInstructions(instruction *instructions) {
    return 0;
}

// Load compiled instructions from a file at path.
instructionContainer *loadInstructions(char *path) {
    int i;
    FILE *f;
    char buffer;
    instructionContainer *container;

    // If the file can't be opened, return NULL.
    if ((f = fopen(path, "r")) == NULL) {
        return NULL;
    }

    if ((container = calloc(1, sizeof(InstructionContainer))) == NULL) {
        fclose(f);

        return NULL;
    }

    // Count the number of lines in the file.
    while (fscanf(f, "%c", &buffer) != EOF) {
        if (buffer == '\n') {
            if (container->instructionCount < MAX_LINES) {
                container->instructionCount++;
            }
            // If the maximum number of lines has been reached, close the
            // file and return NULL.
            else {
                fclose(f);

                return NULL;
            }
        }
    }

    // Create an array of instructions of length count. If the allocation fails,
    // close the file and return NULL.
    container->instructions = malloc(sizeof(instruction) * container->instructionCount);
    if (container->instructions == NULL) {
        fclose(f);
        free(container);

        return NULL;
    }

    // Reset the file pointer to the beginning of the file.
    fseek(f, 0, SEEK_SET);

    // For each line in the file, set the appropriate values for the corresponding
    // location in the new instructions array.
    for (i = 0; i < count; i++) {
        fscanf(f, "%d %d %d %d\n", &instructions[i].opCode,
                                   &instructions[i].RField,
                                   &instructions[i].LField,
                                   &instructions[i].MField);
    }

    fclose(f);

    return instructions;
}
