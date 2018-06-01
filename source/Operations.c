#include <stdio.h>
#include "VirtualMachine.h"

// Load a literal value into register R.
int opLiteral(CPU *cpu) {
    cpu->registers[cpu->instRegister.RField] = cpu->instRegister.MField;
        
    return OP_SUCCESS;
}

int opReturn(CPU *cpu) {
}

int opLoad(CPU *cpu) {
}

int opStore(CPU *cpu) {
}

int opCall(CPU *cpu) {
}

int opAllocate(CPU *cpu) {
}

// Update programCounter to value of M field.
int opJump(CPU *cpu) {
    cpu->programCounter = cpu->instRegister.MField;

    return OP_SUCCESS;
}

// Update programCounter to value of M field if register R is zero.
int opConditionalJump(CPU *cpu) {
    if (cpu->registers[cpu->instRegister.RField] == 0) {
        cpu->programCounter = cpu->instRegister.MField;
    }

    return OP_SUCCESS;
}

// System calls capable of printing, scanning, and ending program.
int opSystemCall(CPU *cpu) {
    switch (cpu->instRegister.MField) {
        case 1:
            printf("%d\n", cpu->registers[cpu->instRegister.RField]);
            return OP_SUCCESS;
    
        case 2:
            scanf("%d", &cpu->registers[cpu->instRegister.RField]);
            return OP_SUCCESS;
       
        case 3:
            return KILL_PROGRAM;
        
        default:
            return OP_FAILURE;
    }
}

// Negate register L and save in register R.
int opNegate(CPU *cpu) {
    cpu->registers[cpu->instRegister.RField] = 0 - cpu->registers[cpu->instRegister.LField];

    return OP_SUCCESS;
}

// Add registers L and M into register R.
int opAdd(CPU *cpu) {
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] +
                                               cpu->registers[cpu->instRegister.MField];
    return OP_SUCCESS;
}

// Subract register M from register L and store in register R.
int opSubtract(CPU *cpu) {
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] -
                                               cpu->registers[cpu->instRegister.MField];
    return OP_SUCCESS;
}

// Multiply registers L and M and store in register R.
int opMultiply(CPU *cpu) {
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] *
                                               cpu->registers[cpu->instRegister.MField];
    return OP_SUCCESS;
}

// Divide register L by register M and store in register R.
int opDivide(CPU *cpu) {
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] /
                                               cpu->registers[cpu->instRegister.MField];
    return OP_SUCCESS;
}

// Store 1 in register R if register R is odd, else store 0.
int opIsOdd(CPU *cpu) {
    if ((cpu->registers[cpu->instRegister.RField] % 2) != 0) {
        cpu->registers[cpu->instRegister.RField] = 1;
    }
    else {
        cpu->registers[cpu->instRegister.RField] = 0;
    }
    
    return OP_SUCCESS;
}

// Store remainder of division of register L by register M in register R.
int opModulus(CPU *cpu) {
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] %
                                               cpu->registers[cpu->instRegister.MField];
    return OP_SUCCESS;
}

// Store 1 in register R if registers L and M are equal, else store 0.
int opIsEqual(CPU *cpu) {
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] ==
                                                cpu->registers[cpu->instRegister.MField]);
    return OP_SUCCESS;
}

// Store 1 in register R if registers L and M are not equal, else store 0.
int opIsNotEqual(CPU *cpu) {
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] !=
                                                cpu->registers[cpu->instRegister.MField]);
    return OP_SUCCESS;
}

// Store 1 in register R if register L is less than register M, else store 0.
int opIsLessThan(CPU *cpu) {
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] <
                                                cpu->registers[cpu->instRegister.MField]);
    return OP_SUCCESS;
}

// Store 1 in register R if register L is less than or equal to register M, else store 0.
int opIsLessThanOrEqualTo(CPU *cpu) {
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] <=
                                                cpu->registers[cpu->instRegister.MField]);
    return OP_SUCCESS;
}

// Store 1 in register R if register L is greater than to register M, else store 0.
int opIsGreaterThan(CPU *cpu) {
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] >
                                                cpu->registers[cpu->instRegister.MField]);
    return OP_SUCCESS;
}

// Store 1 in register R if register L is greater than or equal to register M, else store 0.
int opIsGreaterThanOrEqualTo(CPU *cpu) {
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] >=
                                                cpu->registers[cpu->instRegister.MField]);
    return OP_SUCCESS;
}
