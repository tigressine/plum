#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "VirtualMachine.h"

// Main function and machine entry point.
int main(int argsCount, char **argsVector) {
    int i;
    int traceToggles;
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
   
    // Scan extra arguments for tracing flags. Use the magic of bit fiddling to cram
    // all the flags into a single integer. This is more so I don't have to pass multiple
    // integers to my print functions--not so much a space thing.
    traceToggles = 0;
    for (i = 2; i < argsCount; i++) {
        if (strcmp(argsVector[i], "--trace_cpu") == 0) {
            traceToggles |= TRACE_CPU;
        }
        else if (strcmp(argsVector[i], "--trace_all") == 0) {
            traceToggles |= TRACE_CPU;
            traceToggles |= TRACE_RECORDS;
            traceToggles |= TRACE_REGISTERS;
        }
        else if (strcmp(argsVector[i], "--trace_records") == 0) {
            traceToggles |= TRACE_CPU;
            traceToggles |= TRACE_RECORDS;
        }
        else if (strcmp(argsVector[i], "--trace_registers") == 0) {
            traceToggles |= TRACE_CPU;
            traceToggles |= TRACE_REGISTERS;
        }
    }
    
    // Attempt to process the instructions. If the instructions are not processed
    // correctly, yell about it. 
    if ((processInstructions(instructions, instructionCount, traceToggles)) == OP_FAILURE) {
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
        cpu->instructionCount = instructionCount;
    }

    // Return either the object or NULL based on the results of calloc().
    return cpu;
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
int processInstructions(instruction *instructions, int instructionCount, int traceToggles) {
    int i;
    CPU *cpu;
    int executeReturn;
    recordStack *stack;

    if (instructions == NULL || instructionCount == 0) {
        return OP_FAILURE;
    }

    // If CPU can't be allocated, return OP_FAILURE.
    if ((cpu = createCPU(instructionCount)) == NULL) {
        return OP_FAILURE;
    }

    if ((stack = initializeRecordStack()) == NULL) {
        freeCPU(cpu);

        return OP_FAILURE;
    }
  
    // Push an initial record onto the stack for the main environment.
    pushRecord(cpu, stack);
    
    if (traceToggles) {
        printStackTraceHeader(traceToggles);
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
        if ((executeReturn = executeInstruction(cpu, stack)) == OP_FAILURE) {
            freeCPU(cpu);
            
            return OP_FAILURE;
        }

        if (traceToggles) {
            printStackTraceLine(cpu, stack, traceToggles);
        }
    }

    // Stay memory safe!
    freeCPU(cpu);
    destroyRecordStack(stack);

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
int executeInstruction(CPU *cpu, recordStack *stack) {
    if (cpu == NULL || stack == NULL) {
        return OP_FAILURE;
    }

    // Execute the corresponding function for the instruction loaded into the
    // instRegister. Instruction opCodes are #defined for readability.
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
            return OP_FAILURE;
    }
}

// Encapsulating function to free an instruction array.
int freeInstructions(instruction *instructions) {
    free(instructions);

    return OP_SUCCESS;
}

// Print out a nice header for the stack trace.
void printStackTraceHeader(int traceToggles) {
    printf("Program stack trace:\n");
    printf("OP R  L  M     PC    |");

    if (traceToggles & TRACE_RECORDS) {
        printf(" RV  RA   (LOCALS) |");
    }
    if (traceToggles & TRACE_REGISTERS) {
        printf(" REGS");
    }
    printf("\n");

    printf("----------------------");
    if (traceToggles & TRACE_RECORDS) {
        printf("--------------------");
    }
    if (traceToggles & TRACE_REGISTERS) {
        printf("-------");
    }
    printf("\n");
}

// Print out all associated objects with the program.
void printStackTraceLine(CPU *cpu, recordStack *stack, int traceToggles) {
    if (cpu == NULL || stack == NULL) {
        return;
    }
    
    printCPU(cpu);
    if (traceToggles & TRACE_RECORDS) {
        printRecords(stack->currentRecord);
    }
    if (traceToggles & TRACE_REGISTERS) {
        printRegisters(cpu);
    }
    printf("\n");
}

// Print registers of a CPU.
void printRegisters(CPU *cpu) {
    int i;
    
    if (cpu == NULL) {
        return;
    }
    
    for (i = 0; i < REGISTER_COUNT; i++) {
        printf("%d%s", cpu->registers[i], (i < REGISTER_COUNT - 1) ? " " : "");
    }
}

// Print CPU instruction and program counter.
void printCPU(CPU *cpu) {
    if (cpu == NULL) {
        return;
    }

    printf("%-2d %-2d %-2d %-5d %-5d | ", cpu->instRegister.opCode,
                                          cpu->instRegister.RField,
                                          cpu->instRegister.LField,
                                          cpu->instRegister.MField,
                                          cpu->programCounter);
}

// Recursively print a stack of records.
void printRecords(recordStackItem *record) {
    int i;
    
    if (record == NULL) {
        return;
    }
   
    // Recursively print the rest of the stack first.
    printRecords(record->dynamicLink);
    printf("%-3d %-5d", record->returnValue, record->returnAddress);

    for (i = 0; i < record->localCount; i++) {
        printf(" %-3d", record->locals[i]);
    }
    printf(" | ");
}
