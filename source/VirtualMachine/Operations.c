#include <stdio.h>
#include "VirtualMachine.h"

// Check if provided register is in bounds.
int invalidRegister(int reg) {
    return (reg < 0 || reg >= REGISTER_COUNT);
}

// Check that all registers in provided instruction are valid and that the CPU exists.
int invalidCPU(CPU *cpu) {
    return (cpu == NULL ||
            cpu->registers == NULL ||
            invalidRegister(cpu->instRegister.RField) ||
            invalidRegister(cpu->instRegister.LField) ||
            invalidRegister(cpu->instRegister.MField));
}

// Load a literal value into register R.
int opLiteral(CPU *cpu) {
    if (cpu == NULL || cpu->registers == NULL || invalidRegister(cpu->instRegister.RField)) {
        return OP_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = cpu->instRegister.MField;
        
    return OP_SUCCESS;
}

// Revert programCounter to the top activation record's returnAddress, then pops
// the top record.
int opReturn(CPU *cpu, recordStack *stack) {
    if (cpu == NULL || stack == NULL || stack->currentRecord == NULL) {
        return OP_FAILURE;
    }

    cpu->programCounter = stack->currentRecord->returnAddress;
    
    return popRecord(stack);
}

// Load a value from an activation record into a register.
int opLoad(CPU *cpu, recordStack *stack) {
    recordStackItem *desiredRecord;
    int index;
    
    if (cpu == NULL ||
        stack == NULL ||
        cpu->registers == NULL ||
        invalidRegister(cpu->instRegister.RField)) {
        
        return OP_FAILURE;
    }

    // Get the static parent of the top level record, L levels down.
    if ((desiredRecord = getStaticParent(stack, cpu->instRegister.LField)) == NULL) {
        return OP_FAILURE;
    }

    index = cpu->instRegister.MField;
    // If index is zero, desired value is returnValue.
    if (index == 0) {
        cpu->registers[cpu->instRegister.RField] = desiredRecord->returnValue;
    }
    else {
        // Adjust index by the number of static space in each activation record.
        index -= INT_OFFSET;
        
        // If index is valid for the locals array, retrieve a value from said array.
        if (index >= 0 && index < desiredRecord->localCount) {
            cpu->registers[cpu->instRegister.RField] = desiredRecord->locals[index];
        }
        // Otherwise value is bad, so operation fails.
        else {
            return OP_FAILURE;
        }
    }
    
    return OP_SUCCESS;
}

// Load a value from a register into an activation record in the stack.
int opStore(CPU *cpu, recordStack *stack) {
    recordStackItem *desiredRecord;
    int index;

    if (cpu == NULL ||
        stack == NULL ||
        cpu->registers == NULL ||
        invalidRegister(cpu->instRegister.RField)) {
        
        return OP_FAILURE;
    }

    // Get the static parent of the top level record, L levels down.
    if ((desiredRecord = getStaticParent(stack, cpu->instRegister.LField)) == NULL) {
        return OP_FAILURE;
    }

    index = cpu->instRegister.MField;
    if (index == 0) {
        desiredRecord->returnValue = cpu->registers[cpu->instRegister.RField];
    }
    else {
        index -= INT_OFFSET;
        
        if (index >= 0 && index < desiredRecord->localCount) {
            desiredRecord->locals[index] = cpu->registers[cpu->instRegister.RField];
        }
        // The index is either out of bounds of the locals array, or the user is
        // trying to override some of the other activation record fields (like the
        // dynamic link) and this isn't allowed.
        else {
            return OP_FAILURE;
        }
    }

    return OP_SUCCESS;
}

// Push a new activation record environment onto the stack.
int opCall(CPU *cpu, recordStack *stack) {
    if (cpu == NULL || stack == NULL) {
        return OP_FAILURE;
    }

    pushRecord(cpu, stack);
    // Program jumps to subroutine.
    cpu->programCounter = cpu->instRegister.MField;

    return OP_SUCCESS;
}

// Allocate locals in top level activation record.
int opAllocate(CPU *cpu, recordStack *stack) {
    if (cpu == NULL || stack == NULL) {
        return OP_FAILURE;
    }

    return allocateLocals(stack->currentRecord, cpu->instRegister.MField - INT_OFFSET);
}

// Update programCounter to value of M field.
int opJump(CPU *cpu) {
    if (cpu == NULL) {
        return OP_FAILURE;
    }

    cpu->programCounter = cpu->instRegister.MField;

    return OP_SUCCESS;
}

// Update programCounter to value of M field if register R is zero.
int opConditionalJump(CPU *cpu) {
    if (cpu == NULL || invalidRegister(cpu->instRegister.RField)) {
        return OP_FAILURE;
    }

    if (cpu->registers[cpu->instRegister.RField] == 0) {
        cpu->programCounter = cpu->instRegister.MField;
    }

    return OP_SUCCESS;
}

// System calls capable of printing, scanning, and ending program.
int opSystemCall(CPU *cpu) {
    if (cpu == NULL) {
        return OP_FAILURE;
    }

    switch (cpu->instRegister.MField) {
        // System call to print from a register.
        case 1:
            if (invalidRegister(cpu->instRegister.RField)) {
                return OP_FAILURE;
            }

            printf("%d\n", cpu->registers[cpu->instRegister.RField]);
            return OP_SUCCESS;
    
        // System call to read data from user into a register.
        case 2:
            if (invalidRegister(cpu->instRegister.RField)) {
                return OP_FAILURE;
            }
            
            scanf("%d", &cpu->registers[cpu->instRegister.RField]);
            return OP_SUCCESS;
       
        // System call to terminate the program.
        case 3:
            cpu->programCounter = 0;

            return KILL_PROGRAM;
        
        default:
            return OP_FAILURE;
    }
}

// Negate register L and save in register R.
int opNegate(CPU *cpu) {
   if (invalidCPU(cpu)) { 
        return OP_FAILURE;
    }

    cpu->registers[cpu->instRegister.RField] = 0 - cpu->registers[cpu->instRegister.LField];

    return OP_SUCCESS;
}

// Add registers L and M into register R.
int opAdd(CPU *cpu) {
    if (invalidCPU(cpu)) {
        return OP_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] +
                                               cpu->registers[cpu->instRegister.MField];
    return OP_SUCCESS;
}

// Subract register M from register L and store in register R.
int opSubtract(CPU *cpu) {
    if (invalidCPU(cpu)) {
        return OP_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] -
                                               cpu->registers[cpu->instRegister.MField];
    return OP_SUCCESS;
}

// Multiply registers L and M and store in register R.
int opMultiply(CPU *cpu) {
    if (invalidCPU(cpu)) {
        return OP_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] *
                                               cpu->registers[cpu->instRegister.MField];
    return OP_SUCCESS;
}

// Divide register L by register M and store in register R.
int opDivide(CPU *cpu) {
    if (invalidCPU(cpu) || cpu->registers[cpu->instRegister.MField] == 0) {
        return OP_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] /
                                               cpu->registers[cpu->instRegister.MField];
    return OP_SUCCESS;
}

// Store 1 in register R if register R is odd, else store 0.
int opIsOdd(CPU *cpu) {
    if (invalidCPU(cpu)) {
        return OP_FAILURE;
    }
    
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
    if (invalidCPU(cpu)) {
        return OP_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] %
                                               cpu->registers[cpu->instRegister.MField];
    return OP_SUCCESS;
}

// Store 1 in register R if registers L and M are equal, else store 0.
int opIsEqual(CPU *cpu) {
    if (invalidCPU(cpu)) {
        return OP_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] ==
                                                cpu->registers[cpu->instRegister.MField]);
    return OP_SUCCESS;
}

// Store 1 in register R if registers L and M are not equal, else store 0.
int opIsNotEqual(CPU *cpu) {
    if (invalidCPU(cpu)) {
        return OP_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] !=
                                                cpu->registers[cpu->instRegister.MField]);
    return OP_SUCCESS;
}

// Store 1 in register R if register L is less than register M, else store 0.
int opIsLessThan(CPU *cpu) {
    if (invalidCPU(cpu)) {
        return OP_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] <
                                                cpu->registers[cpu->instRegister.MField]);
    return OP_SUCCESS;
}

// Store 1 in register R if register L is less than or equal to register M, else store 0.
int opIsLessThanOrEqualTo(CPU *cpu) {
    if (invalidCPU(cpu)) {
        return OP_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] <=
                                                cpu->registers[cpu->instRegister.MField]);
    return OP_SUCCESS;
}

// Store 1 in register R if register L is greater than to register M, else store 0.
int opIsGreaterThan(CPU *cpu) {
    if (invalidCPU(cpu)) {
        return OP_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] >
                                                cpu->registers[cpu->instRegister.MField]);
    return OP_SUCCESS;
}

// Store 1 in register R if register L is greater than or equal to register M, else store 0.
int opIsGreaterThanOrEqualTo(CPU *cpu) {
    if (invalidCPU(cpu)) {
        return OP_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] >=
                                                cpu->registers[cpu->instRegister.MField]);
    return OP_SUCCESS;
}
