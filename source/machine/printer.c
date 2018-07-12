#include <stdio.h>
#include "machine.h"

// Print out a nice header for the stack trace.
void printStackTraceHeader(int options) {
    printf("Program stack trace:\n");
    printf("OP  R  L  M     PC    |");

    if (checkOption(&options, OPTION_TRACE_RECORDS)) {
        printf(" RV  RA   (LOCALS) |");
    }
    if (checkOption(&options, OPTION_TRACE_REGISTERS)) {
        printf(" REGS");
    }
    printf("\n");

    printf("----------------------");
    if (checkOption(&options, OPTION_TRACE_RECORDS)) {
        printf("--------------------");
    }
    if (checkOption(&options, OPTION_TRACE_REGISTERS)) {
        printf("-------");
    }
    printf("\n");
}

// Print out all associated objects with the program.
void printStackTraceLine(CPU *cpu, recordStack *stack, int options) {
    if (cpu == NULL || stack == NULL) {
        printError(ERROR_NULL_CHECK);
        
        return;
    }
    
    printCPU(cpu);
    if (checkOption(&options, OPTION_TRACE_RECORDS)) {
        printRecords(stack->currentRecord);
    }
    if (checkOption(&options, OPTION_TRACE_REGISTERS)) {
        printRegisters(cpu);
    }
    printf("\n");
}

// Print registers of a CPU.
void printRegisters(CPU *cpu) {
    int i;
    
    if (cpu == NULL) {
        printError(ERROR_NULL_CHECK);
        
        return;
    }
    
    for (i = 0; i < REGISTER_COUNT; i++) {
        printf("%d%s", cpu->registers[i], (i < REGISTER_COUNT - 1) ? " " : "");
    }
}

// Print CPU instruction and program counter.
void printCPU(CPU *cpu) {
    if (cpu == NULL) {
        printError(ERROR_NULL_CHECK);
        
        return;
    }

    // Print the appropriate operation code.
    switch (cpu->instRegister.opCode) {
        case LIT: printf("LIT "); break; 
        case RTN: printf("RTN "); break; 
        case LOD: printf("LOD "); break; 
        case STO: printf("STO "); break; 
        case CAL: printf("CAL "); break; 
        case INC: printf("INC "); break; 
        case JMP: printf("JMP "); break; 
        case JPC: printf("JPC "); break; 
        case SIO: printf("SIO "); break; 
        case NEG: printf("NEG "); break; 
        case ADD: printf("ADD "); break; 
        case SUB: printf("SUB "); break; 
        case MUL: printf("MUL "); break; 
        case DIV: printf("DIV "); break; 
        case ODD: printf("ODD "); break; 
        case MOD: printf("MOD "); break; 
        case EQL: printf("EQL "); break; 
        case NEQ: printf("NEQ "); break; 
        case LSS: printf("LSS "); break; 
        case LEQ: printf("LEQ "); break; 
        case GTR: printf("GTR "); break; 
        case GEQ: printf("GEQ "); break; 
    }

    printf("%-2d %-2d %-5d %-5d | ", cpu->instRegister.RField,
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
