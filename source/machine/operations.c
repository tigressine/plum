// Part of Plum by Tiger Sachse.

#include <stdio.h>
#include <stdarg.h>
#include "machine.h"

// Return the validity of a particular register.
int invalidRegister(int index) {
    if (index < 0 || index >= REGISTER_COUNT) {
        printError(ERROR_REGISTER_OUT_OF_BOUNDS, index);

        return SIGNAL_TRUE;
    }
    else {
        return SIGNAL_FALSE;
    }
}

// Return validity of CPU.
int invalidCPUState(CPU *cpu, int registerCheck) {
    if (cpu == NULL) {
        printError(ERROR_NULL_POINTER);

        return SIGNAL_TRUE;
    }

    // Check either only the RField or all three fields.
    if (registerCheck == 1) {
        if (invalidRegister(cpu->instRegister.RField)) {
            return SIGNAL_TRUE;
        }
    }
    else if (registerCheck == 3) {
        if (invalidRegister(cpu->instRegister.RField) ||
            invalidRegister(cpu->instRegister.LField) ||
            invalidRegister(cpu->instRegister.MField)) {
       
            return SIGNAL_TRUE;
        }
    }
        
    return SIGNAL_FALSE;
}

// Load a literal value into register R.
int operationLiteral(CPU *cpu) {
    if (invalidCPUState(cpu, 1)) {
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = cpu->instRegister.MField;
        
    return SIGNAL_SUCCESS;
}

// Revert programCounter to the top activation record's returnAddress, then pops
// the top record.
int operationReturn(CPU *cpu, RecordStack *stack) {
    if (invalidCPUState(cpu, 0)) {
        return SIGNAL_FAILURE;
    }
    
    if (stack == NULL) {
        printError(ERROR_NULL_POINTER);

        return SIGNAL_FAILURE;
    }

    cpu->programCounter = stack->currentRecord->returnAddress;
    
    return popRecord(stack);
}

// Load a value from an activation record into a register.
int operationLoad(CPU *cpu, RecordStack *stack) {
    int index;
    RecordStackItem *desiredRecord;
    
    if (invalidCPUState(cpu, 1)) {
        return SIGNAL_FAILURE;
    }

    if (stack == NULL) {
        printError(ERROR_NULL_POINTER);

        return SIGNAL_FAILURE;
    }

    // Get the static parent of the top level record, L levels down.
    if ((desiredRecord = getStaticParent(stack, cpu->instRegister.LField)) == NULL) {
        printError(ERROR_NO_STATIC_PARENT);
        
        return SIGNAL_FAILURE;
    }

    // If index is zero, desired value is returnValue.
    index = cpu->instRegister.MField;
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
            printError(ERROR_LOCAL_INDEX_OUT_OF_BOUNDS, index);

            return SIGNAL_FAILURE;
        }
    }
    
    return SIGNAL_SUCCESS;
}

// Load a value from a register into an activation record in the stack.
int operationStore(CPU *cpu, RecordStack *stack) {
    int index;
    RecordStackItem *desiredRecord;

    if (invalidCPUState(cpu, 1)) {
        return SIGNAL_FAILURE;
    }
    
    if (stack == NULL) {
        printError(ERROR_NULL_POINTER);

        return SIGNAL_FAILURE;
    }

    // Get the static parent of the top level record, L levels down.
    if ((desiredRecord = getStaticParent(stack, cpu->instRegister.LField)) == NULL) {
        printError(ERROR_NO_STATIC_PARENT);
        
        return SIGNAL_FAILURE;
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
            printError(ERROR_LOCAL_INDEX_OUT_OF_BOUNDS, index);

            return SIGNAL_FAILURE;
        }
    }

    return SIGNAL_SUCCESS;
}

// Push a new activation record environment onto the stack.
int operationCall(CPU *cpu, RecordStack *stack) {
    if (invalidCPUState(cpu, 0)) {
        return SIGNAL_FAILURE;
    }

    if (stack == NULL) {
        printError(ERROR_NULL_POINTER);

        return SIGNAL_FAILURE;
    }
    
    pushRecord(cpu, stack);
    
    // Program jumps to subroutine.
    cpu->programCounter = cpu->instRegister.MField;

    return SIGNAL_SUCCESS;
}

// Allocate locals in top level activation record.
int operationAllocate(CPU *cpu, RecordStack *stack) {
    if (invalidCPUState(cpu, 0)) {
        return SIGNAL_FAILURE;
    }

    if (stack == NULL) {
        printError(ERROR_NULL_POINTER);

        return SIGNAL_FAILURE;
    }

    return allocateLocals(stack->currentRecord, cpu->instRegister.MField - INT_OFFSET);
}

// Update programCounter to value of M field.
int operationJump(CPU *cpu) {
    if (invalidCPUState(cpu, 0)) {
        return SIGNAL_FAILURE;
    }

    cpu->programCounter = cpu->instRegister.MField;

    return SIGNAL_SUCCESS;
}

// Update programCounter to value of M field if register R is zero.
int operationConditionalJump(CPU *cpu) {
    if (invalidCPUState(cpu, 1)) {
        return SIGNAL_FAILURE;
    }

    if (cpu->registers[cpu->instRegister.RField] == 0) {
        cpu->programCounter = cpu->instRegister.MField;
    }

    return SIGNAL_SUCCESS;
}

// System calls capable of printing, scanning, and ending program.
int operationSystemCall(CPU *cpu) {
    if (invalidCPUState(cpu, 0)) {
        return SIGNAL_FAILURE;
    }

    if (cpu->instRegister.MField == CALL_PRINT) {
        if (invalidRegister(cpu->instRegister.RField)) {
            return SIGNAL_FAILURE;
        }
        printf("%d\n", cpu->registers[cpu->instRegister.RField]);
        
        return SIGNAL_SUCCESS;
    }
    else if (cpu->instRegister.MField == CALL_SCAN) {
        if (invalidRegister(cpu->instRegister.RField)) {
            return SIGNAL_FAILURE;
        }
        scanf("%d", &cpu->registers[cpu->instRegister.RField]);
        
        return SIGNAL_SUCCESS;
    }
    else if (cpu->instRegister.MField == CALL_KILL) {
        cpu->programCounter = 0;

        return SIGNAL_KILL;
    }
    else {
        printError(ERROR_ILLEGAL_SYSTEM_CALL, cpu->instRegister.MField);

        return SIGNAL_FAILURE;
    }
}

// Negate register L and save in register R.
int operationNegate(CPU *cpu) {
    if (invalidCPUState(cpu, 3)) {
        return SIGNAL_FAILURE;
    }

    cpu->registers[cpu->instRegister.RField] = 0 - cpu->registers[cpu->instRegister.LField];

    return SIGNAL_SUCCESS;
}

// Add registers L and M into register R.
int operationAdd(CPU *cpu) {
    if (invalidCPUState(cpu, 3)) {
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] +
                                               cpu->registers[cpu->instRegister.MField];
    return SIGNAL_SUCCESS;
}

// Subract register M from register L and store in register R.
int operationSubtract(CPU *cpu) {
    if (invalidCPUState(cpu, 3)) {
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] -
                                               cpu->registers[cpu->instRegister.MField];
    return SIGNAL_SUCCESS;
}

// Multiply registers L and M and store in register R.
int operationMultiply(CPU *cpu) {
    if (invalidCPUState(cpu, 3)) {
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] *
                                               cpu->registers[cpu->instRegister.MField];
    return SIGNAL_SUCCESS;
}

// Divide register L by register M and store in register R.
int operationDivide(CPU *cpu) {
    if (invalidCPUState(cpu, 3)) {
        return SIGNAL_FAILURE;
    }
    
    if (cpu->registers[cpu->instRegister.MField] == 0) {
        printError(ERROR_DIVIDE_BY_ZERO);
        
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] /
                                               cpu->registers[cpu->instRegister.MField];
    return SIGNAL_SUCCESS;
}

// Store 1 in register R if register R is odd, else store 0.
int operationIsOdd(CPU *cpu) {
    if (invalidCPUState(cpu, 3)) {
        return SIGNAL_FAILURE;
    }
    
    if ((cpu->registers[cpu->instRegister.RField] % 2) != 0) {
        cpu->registers[cpu->instRegister.RField] = 1;
    }
    else {
        cpu->registers[cpu->instRegister.RField] = 0;
    }
    
    return SIGNAL_SUCCESS;
}

// Store remainder of division of register L by register M in register R.
int operationModulus(CPU *cpu) {
    if (invalidCPUState(cpu, 3)) {
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] %
                                               cpu->registers[cpu->instRegister.MField];
    return SIGNAL_SUCCESS;
}

// Store 1 in register R if registers L and M are equal, else store 0.
int operationIsEqual(CPU *cpu) {
    if (invalidCPUState(cpu, 3)) {
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] ==
                                                cpu->registers[cpu->instRegister.MField]);
    return SIGNAL_SUCCESS;
}

// Store 1 in register R if registers L and M are not equal, else store 0.
int operationIsNotEqual(CPU *cpu) {
    if (invalidCPUState(cpu, 3)) {
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] !=
                                                cpu->registers[cpu->instRegister.MField]);
    return SIGNAL_SUCCESS;
}

// Store 1 in register R if register L is less than register M, else store 0.
int operationIsLessThan(CPU *cpu) {
    if (invalidCPUState(cpu, 3)) {
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] <
                                                cpu->registers[cpu->instRegister.MField]);
    return SIGNAL_SUCCESS;
}

// Store 1 in register R if register L is less than or equal to register M, else store 0.
int operationIsLessThanOrEqualTo(CPU *cpu) {
    if (invalidCPUState(cpu, 3)) {
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] <=
                                                cpu->registers[cpu->instRegister.MField]);
    return SIGNAL_SUCCESS;
}

// Store 1 in register R if register L is greater than to register M, else store 0.
int operationIsGreaterThan(CPU *cpu) {
    if (invalidCPUState(cpu, 3)) {
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] >
                                                cpu->registers[cpu->instRegister.MField]);
    return SIGNAL_SUCCESS;
}

// Store 1 in register R if register L is greater than or equal to register M, else store 0.
int operationIsGreaterThanOrEqualTo(CPU *cpu) {
    if (invalidCPUState(cpu, 3)) {
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] >=
                                                cpu->registers[cpu->instRegister.MField]);
    return SIGNAL_SUCCESS;
}
