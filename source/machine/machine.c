#include <stdio.h>
#include <stdlib.h>
#include "machine.h"

int startMachine(char *inFile, int options) {
    int instructionCount;
    instruction *instructions;

    if (inFile == NULL) {
        //printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    if ((instructionCount = countInstructions(inFile)) == SIGNAL_FAILURE) {
        return SIGNAL_FAILURE;
    }
    
    if ((instructions = loadInstructions(inFile, instructionCount)) == NULL) {
        return SIGNAL_FAILURE;
    }
    
    if ((processInstructions(instructions, instructionCount, options)) == SIGNAL_FAILURE) {
        destroyInstructions(instructions);

        return SIGNAL_FAILURE;
    }
    else {
        destroyInstructions(instructions);

        return SIGNAL_SUCCESS;
    }
}

CPU *createCPU(int instructionCount) {
    CPU *cpu;

    if ((cpu = calloc(1, sizeof(CPU))) != NULL) {
        cpu->instructionCount = instructionCount;
    }

    // Return either the object or NULL based on the results of calloc().
    return cpu;
}

// Encapsulating method to free any created CPUs.
int destroyCPU(CPU *cpu) {
    free(cpu);

    return SIGNAL_SUCCESS; 
}

// Count instructions in provided file. Needed for programCounter bounds checks.
int countInstructions(char *filename) {
    FILE *f;
    int count;
    char buffer;

    if (filename == NULL) {
        //printError(ERROR_NULL_CHECK);
        
        return SIGNAL_FAILURE;
    }

    // If the file can't be opened, return SIGNAL_FAILURE.
    if ((f = fopen(filename, "r")) == NULL) {
        return SIGNAL_FAILURE;
    }

    // Count the number of lines in the file.
    count = 0;
    while (fscanf(f, "%c", &buffer) != EOF) {
        if (buffer == '\n') {
            if (count < MAX_LINES) {
                count++;
            }
            // If the maximum number of lines has been reached, close the
            // file and return SIGNAL_FAILURE.
            else {
                fclose(f);
                //printError(ERROR_FILE_TOO_LONG, filename);

                return SIGNAL_FAILURE;
            }
        }
    }
    
    fclose(f);

    return count;
}

// Load compiled instructions from a file at filename.
instruction *loadInstructions(char *filename, int instructionCount) {
    int i;
    FILE *f;
    instruction *instructions;

    if (filename == NULL) {
        //printError(ERROR_NULL_CHECK);
        
        return NULL;
    }

    // If the file can't be opened, return NULL.
    if ((f = fopen(filename, "r")) == NULL) {
        //printError(ERROR_FILE_NOT_FOUND, filename);
        
        return NULL;
    }

    // Create an array of instructions of length instructionCount. If the allocation fails,
    // close the file and return NULL.
    if ((instructions = malloc(sizeof(instruction) * instructionCount)) == NULL) {
        fclose(f);
        //printError(ERROR_OUT_OF_MEMORY);

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
int processInstructions(instruction *instructions, int instructionCount, int options) {
    int i;
    CPU *cpu;
    int executeReturn;
    recordStack *stack;

    if (instructions == NULL || instructionCount == 0) {
        //printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    // If CPU can't be allocated, return SIGNAL_FAILURE.
    if ((cpu = createCPU(instructionCount)) == NULL) {
        //printError(ERROR_OUT_OF_MEMORY);
        
        return SIGNAL_FAILURE;
    }

    if ((stack = initializeRecordStack()) == NULL) {
        destroyCPU(cpu);
        //printError(ERROR_OUT_OF_MEMORY);
        
        return SIGNAL_FAILURE;
    }
  
    // Push an initial record onto the stack for the main environment.
    pushRecord(cpu, stack);
   
    /*
    if (options) {//this will be wrong, need to use checks
        printStackTraceHeader(options);
    }*/

    // Perform successive fetches and executes for the array of instructions
    // until an error occurs or a SIGNAL_KILL system call is made.
    executeReturn = SIGNAL_SUCCESS;
    while (executeReturn != SIGNAL_KILL) {
        // Check that fetchInstruction is successful.
        if (fetchInstruction(cpu, instructions) == SIGNAL_FAILURE) {
            destroyCPU(cpu);
            
            return SIGNAL_FAILURE;
        }

        // Check that executeInstruction is successful.
        if ((executeReturn = executeInstruction(cpu, stack)) == SIGNAL_FAILURE) {
            destroyCPU(cpu);
            
            return SIGNAL_FAILURE;
        }

        /*
        if (options) {///again this will be wrong
            printStackTraceLine(cpu, stack, options);
        }*/
    }

    // Stay memory safe!
    destroyCPU(cpu);
    destroyRecordStack(stack);

    return SIGNAL_SUCCESS;
}

// Fetch next instruction from instructions and place in the CPU instRegister.
int fetchInstruction(CPU *cpu, instruction *instructions) {
    if (cpu == NULL || instructions == NULL) {
        //printError(ERROR_NULL_CHECK);
        
        return SIGNAL_FAILURE;
    }

    // If the desired instruction (indicated by cpu->programCounter) is not out of
    // bounds, increment the programCounter and load the new instruction into the
    // instRegister.
    if (cpu->programCounter >= 0 && cpu->programCounter < cpu->instructionCount) {
        cpu->instRegister = instructions[cpu->programCounter];
        cpu->programCounter++;

        return SIGNAL_SUCCESS;
    }
    else {
        //printError(ERROR_PROGRAM_COUNTER_OUT_OF_BOUNDS);

        return SIGNAL_FAILURE;
    }
}

// Execute instruction loaded into CPU.
int executeInstruction(CPU *cpu, recordStack *stack) {
    if (cpu == NULL || stack == NULL) {
        //printError(ERROR_NULL_CHECK);
        
        return SIGNAL_FAILURE;
    }

    // Execute the corresponding function for the instruction
    // loaded into the instRegister.
    switch (cpu->instRegister.opCode) {
        case LIT: return opLiteral(cpu);
        case RTN: return opReturn(cpu, stack);
        case LOD: return opLoad(cpu, stack);
        case STO: return opStore(cpu, stack);
        case CAL: return opCall(cpu, stack);
        case INC: return opAllocate(cpu, stack);
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
            //printError(ERROR_UNKNOWN_OPCODE);
            
            return SIGNAL_FAILURE;
    }
}

// Encapsulating function to free an instruction array.
int destroyInstructions(instruction *instructions) {
    free(instructions);

    return SIGNAL_SUCCESS;
}
