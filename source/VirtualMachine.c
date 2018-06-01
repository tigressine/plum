#include <stdio.h>
#include <stdlib.h>
#include "Stack.h"
#include "Operations.h"
#include "VirtualMachine.h"

// Main function and machine entry point.
int main(int argsCount, char **argsVector) {
    int i;
    int instructionCount;
    instruction *instructions;

    // If no arguments are passed, yell about it.
    if (argsCount < 2) {
        printf("Please pass a compiled PL/0 file as your first argument.\n");

        return 0;
    }
    // Attempt to count the instructions. If something goes wrong during instruction
    // counting, yell about it.
    else if ((instructionCount = countInstructions(argsVector[1])) == OP_FAILURE) {
        printf("File could not be read or the file is too long. Check your file.\n");

        return 0;
    }
    // Attempt to load the instructions. If something goes wrong during instruction
    // loading, yell about it.
    else if ((instructions = loadInstructions(argsVector[1], instructionCount)) == NULL) {
        printf("File could not be read, or program ran out of memory loading instructions.\n");
    
        return 0;
    }

    // Attempt to process the instructions. If the instructions are not processed
    // correctly, yell about it.
    if ((processInstructions(instructions, instructionCount)) == OP_FAILURE) {
        printf("An error occurred during execution.\n");
    }

    // Don't forget to be memory safe!
    freeInstructions(instructions);

    return 0;
}

// Create a CPU with some default values.
CPU *createCPU(int instructionCount) {
    CPU *cpu;

    if ((cpu = calloc(1, sizeof(CPU))) != NULL) {
        cpu->basePointer = 1;
        cpu->instructionCount = instructionCount;
    }

    // Return either the object or NULL based on the results of calloc().
    return cpu;
}

// Print contents of CPU to screen.
void printCPU(CPU *cpu) {
    int i;

    if (cpu == NULL) {
        return;
    }

    printf("CPU\n===\n");
    printf("SP: %d, BP: %d, PC: %d, IC: %d\n",
           cpu->stackPointer,
           cpu->basePointer, 
           cpu->programCounter,
           cpu->instructionCount);
    printf("Instruction %d, %d, %d, %d\n",
           cpu->instRegister.opCode,
           cpu->instRegister.RField,
           cpu->instRegister.LField,
           cpu->instRegister.MField);
    
    printf("Registers:\n");
    for (i = 0; i < 16; i++) {
        printf("R%02d: %05d%s", i, cpu->registers[i], ((i + 1) % 4 == 0) ? "\n" : " ");
    }
}

// Encapsulating method to free any created CPUs.
int freeCPU(CPU *cpu) {
    free(cpu);

    return OP_SUCCESS; 
}

// Count instructions in provided file. Needed for programCounter bounds checks.
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

// Process the provided instructions using a CPU.
int processInstructions(instruction *instructions, int instructionCount) {
    int i;
    CPU *cpu;
    int executeReturn;

    // If CPU can't be allocated, return OP_FAILURE.
    if ((cpu = createCPU(instructionCount)) == NULL) {
        return OP_FAILURE;
    }
    
    // Perform successive fetches and executes for the array of instructions
    // until an error occurs or a KILL_PROGRAM system call is made.
    executeReturn = OP_SUCCESS;
    while (executeReturn != KILL_PROGRAM) {
        // Check that fetchInstruction is successful.
        if (fetchInstruction(cpu, instructions) == OP_FAILURE) {
            freeCPU(cpu);
            
            return OP_FAILURE;
        }

        // Check that executeInstruction is successful.
        if ((executeReturn = executeInstruction(cpu)) == OP_FAILURE) {
            freeCPU(cpu);
            
            return OP_FAILURE;
        }
        printCPU(cpu);
    }

    // Stay memory safe!
    freeCPU(cpu);

    return OP_SUCCESS;
}

// Fetch next instruction from instructions and place in the CPU instRegister.
int fetchInstruction(CPU *cpu, instruction *instructions) {
    if (cpu == NULL || instructions == NULL) {
        return OP_FAILURE;
    }

    // If the desired instruction (indicated by cpu->programCounter) is not out of
    // bounds, increment the programCounter and load the new instruction into the
    // instRegister.
    if (cpu->programCounter >= 0 && cpu->programCounter < cpu->instructionCount) {
        cpu->instRegister = instructions[cpu->programCounter];
        cpu->programCounter++;

        return OP_SUCCESS;
    }
    else {
        return OP_FAILURE;
    }
}

// Execute instruction loaded into CPU.
int executeInstruction(CPU *cpu) {
    if (cpu == NULL) {
        return OP_FAILURE;
    }

    // Execute the corresponding function for the instruction loaded into the
    // instRegister. Instruction opCodes are #defined for readability.
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

// Encapsulating function to free an instruction array.
int freeInstructions(instruction *instructions) {
    free(instructions);

    return OP_SUCCESS;
}
