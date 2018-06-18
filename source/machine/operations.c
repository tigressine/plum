#include <stdio.h>
#include <stdarg.h>
#include "machine.h"

// Check that provided registers are all valid for the CPU.
int invalidRegisters(int count, ...) {
    int currentRegister;
    va_list registers;
    int i;

    // Initialize the arguments list of the variadic function.
    va_start(registers, count);

    for (i = 0; i < count; i++) {
        // If the next register is out of bounds, return true.
        currentRegister = va_arg(registers, int);
        if (currentRegister < 0 || currentRegister >= REGISTER_COUNT) {
            va_end(registers);

            return 1;
        }
    }
    
    // Close the argument list for the variadic function.
    va_end(registers);

    // All the registers were inbounds. Return false.
    return 0;
}

// Load a literal value into register R.
int operationLiteral(CPU *cpu) {
    if (invalidRegisters(1, cpu->instRegister.RField)) {
        //printError(ERROR_INVALID_REGISTER);
        
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = cpu->instRegister.MField;
        
    return SIGNAL_SUCCESS;
}

// Revert programCounter to the top activation record's returnAddress, then pops
// the top record.
int operationReturn(CPU *cpu, recordStack *stack) {
    cpu->programCounter = stack->currentRecord->returnAddress;
    
    return popRecord(stack);
}

// Load a value from an activation record into a register.
int operationLoad(CPU *cpu, recordStack *stack) {
    recordStackItem *desiredRecord;
    int index;
    
    if (invalidRegisters(1, cpu->instRegister.RField)) {
        //printError(ERROR_INVALID_REGISTER);
        
        return SIGNAL_FAILURE;
    }

    // Get the static parent of the top level record, L levels down.
    if ((desiredRecord = getStaticParent(stack, cpu->instRegister.LField)) == NULL) {
        //printError(ERROR_ILLEGAL_STATIC_PARENT);
        
        return SIGNAL_FAILURE;
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
            //printError(ERROR_OUT_OF_LOCALS);

            return SIGNAL_FAILURE;
        }
    }
    
    return SIGNAL_SUCCESS;
}

// Load a value from a register into an activation record in the stack.
int operationStore(CPU *cpu, recordStack *stack) {
    recordStackItem *desiredRecord;
    int index;

    if (invalidRegisters(1, cpu->instRegister.RField)) {
        //printError(ERROR_INVALID_REGISTER);
        
        return SIGNAL_FAILURE;
    }

    // Get the static parent of the top level record, L levels down.
    if ((desiredRecord = getStaticParent(stack, cpu->instRegister.LField)) == NULL) {
        //printError(ERROR_ILLEGAL_STATIC_PARENT);
        
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
            //printError(ERROR_OUT_OF_LOCALS);

            return SIGNAL_FAILURE;
        }
    }

    return SIGNAL_SUCCESS;
}

// Push a new activation record environment onto the stack.
int operationCall(CPU *cpu, recordStack *stack) {
    pushRecord(cpu, stack);
    
    // Program jumps to subroutine.
    cpu->programCounter = cpu->instRegister.MField;

    return SIGNAL_SUCCESS;
}

// Allocate locals in top level activation record.
int operationAllocate(CPU *cpu, recordStack *stack) {
    return allocateLocals(stack->currentRecord, cpu->instRegister.MField - INT_OFFSET);
}

// Update programCounter to value of M field.
int operationJump(CPU *cpu) {
    cpu->programCounter = cpu->instRegister.MField;

    return SIGNAL_SUCCESS;
}

// Update programCounter to value of M field if register R is zero.
int operationConditionalJump(CPU *cpu) {
    if (invalidRegisters(1, cpu->instRegister.RField)) {
        //printError(ERROR_INVALID_REGISTERS);
        
        return SIGNAL_FAILURE;
    }

    if (cpu->registers[cpu->instRegister.RField] == 0) {
        cpu->programCounter = cpu->instRegister.MField;
    }

    return SIGNAL_SUCCESS;
}

// System calls capable of printing, scanning, and ending program.
int operationSystemCall(CPU *cpu) {
    switch (cpu->instRegister.MField) {
        // System call to print from a register.
        case 1:
            if (invalidRegisters(1, cpu->instRegister.RField)) {
                //printError(ERROR_INVALID_REGISTER);
                
                return SIGNAL_FAILURE;
            }
            printf("%d\n", cpu->registers[cpu->instRegister.RField]);
            
            return SIGNAL_SUCCESS;
    
        // System call to read data from user into a register.
        case 2:
            if (invalidRegisters(1, cpu->instRegister.RField)) {
                //printError(ERROR_INVALID_REGISTER);
                
                return SIGNAL_FAILURE;
            }
            scanf("%d", &cpu->registers[cpu->instRegister.RField]);
            
            return SIGNAL_SUCCESS;
       
        // System call to terminate the program.
        case 3:
            cpu->programCounter = 0;

            return SIGNAL_KILL;
        
        default:
            //printError(ERROR_ILLEGAL_SYSTEM_CALL);

            return SIGNAL_FAILURE;
    }
}

// Negate register L and save in register R.
int operationNegate(CPU *cpu) {
    if (invalidRegisters(3, cpu->instRegister.RField,
                            cpu->instRegister.LField,
                            cpu->instRegister.MField)) { 
        //printError(ERROR_INVALID_REGISTER);
        
        return SIGNAL_FAILURE;
    }

    cpu->registers[cpu->instRegister.RField] = 0 - cpu->registers[cpu->instRegister.LField];

    return SIGNAL_SUCCESS;
}

// Add registers L and M into register R.
int operationAdd(CPU *cpu) {
    if (invalidRegisters(3, cpu->instRegister.RField,
                            cpu->instRegister.LField,
                            cpu->instRegister.MField)) { 
        //printError(ERROR_INVALID_REGISTER);
        
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] +
                                               cpu->registers[cpu->instRegister.MField];
    return SIGNAL_SUCCESS;
}

// Subract register M from register L and store in register R.
int operationSubtract(CPU *cpu) {
    if (invalidRegisters(3, cpu->instRegister.RField,
                            cpu->instRegister.LField,
                            cpu->instRegister.MField)) { 
        //printError(ERROR_INVALID_REGISTER);
        
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] -
                                               cpu->registers[cpu->instRegister.MField];
    return SIGNAL_SUCCESS;
}

// Multiply registers L and M and store in register R.
int operationMultiply(CPU *cpu) {
    if (invalidRegisters(3, cpu->instRegister.RField,
                            cpu->instRegister.LField,
                            cpu->instRegister.MField)) { 
        //printError(ERROR_INVALID_REGISTER);
        
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] *
                                               cpu->registers[cpu->instRegister.MField];
    return SIGNAL_SUCCESS;
}

// Divide register L by register M and store in register R.
int operationDivide(CPU *cpu) {
    if (invalidRegisters(3, cpu->instRegister.RField,
                            cpu->instRegister.LField,
                            cpu->instRegister.MField)) { 
        //printError(ERROR_INVALID_REGISTER);
        
        return SIGNAL_FAILURE;
    }
    
    if (cpu->registers[cpu->instRegister.MField] == 0) {
        //printError(ERROR_DIVIDE_BY_ZERO);
        
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] /
                                               cpu->registers[cpu->instRegister.MField];
    return SIGNAL_SUCCESS;
}

// Store 1 in register R if register R is odd, else store 0.
int operationIsOdd(CPU *cpu) {
    if (invalidRegisters(3, cpu->instRegister.RField,
                            cpu->instRegister.LField,
                            cpu->instRegister.MField)) { 
        //printError(ERROR_INVALID_REGISTER);
        
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
    if (invalidRegisters(3, cpu->instRegister.RField,
                            cpu->instRegister.LField,
                            cpu->instRegister.MField)) { 
        //printError(ERROR_INVALID_REGISTER);
        
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] %
                                               cpu->registers[cpu->instRegister.MField];
    return SIGNAL_SUCCESS;
}

// Store 1 in register R if registers L and M are equal, else store 0.
int operationIsEqual(CPU *cpu) {
    if (invalidRegisters(3, cpu->instRegister.RField,
                            cpu->instRegister.LField,
                            cpu->instRegister.MField)) { 
        //printError(ERROR_INVALID_REGISTER);
        
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] ==
                                                cpu->registers[cpu->instRegister.MField]);
    return SIGNAL_SUCCESS;
}

// Store 1 in register R if registers L and M are not equal, else store 0.
int operationIsNotEqual(CPU *cpu) {
    if (invalidRegisters(3, cpu->instRegister.RField,
                            cpu->instRegister.LField,
                            cpu->instRegister.MField)) { 
        //printError(ERROR_INVALID_REGISTER);
        
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] !=
                                                cpu->registers[cpu->instRegister.MField]);
    return SIGNAL_SUCCESS;
}

// Store 1 in register R if register L is less than register M, else store 0.
int operationIsLessThan(CPU *cpu) {
    if (invalidRegisters(3, cpu->instRegister.RField,
                            cpu->instRegister.LField,
                            cpu->instRegister.MField)) { 
        //printError(ERROR_INVALID_REGISTER);
        
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] <
                                                cpu->registers[cpu->instRegister.MField]);
    return SIGNAL_SUCCESS;
}

// Store 1 in register R if register L is less than or equal to register M, else store 0.
int operationIsLessThanOrEqualTo(CPU *cpu) {
    if (invalidRegisters(3, cpu->instRegister.RField,
                            cpu->instRegister.LField,
                            cpu->instRegister.MField)) { 
        //printError(ERROR_INVALID_REGISTER);
        
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] <=
                                                cpu->registers[cpu->instRegister.MField]);
    return SIGNAL_SUCCESS;
}

// Store 1 in register R if register L is greater than to register M, else store 0.
int operationIsGreaterThan(CPU *cpu) {
    if (invalidRegisters(3, cpu->instRegister.RField,
                            cpu->instRegister.LField,
                            cpu->instRegister.MField)) { 
        //printError(ERROR_INVALID_REGISTER);
        
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] >
                                                cpu->registers[cpu->instRegister.MField]);
    return SIGNAL_SUCCESS;
}

// Store 1 in register R if register L is greater than or equal to register M, else store 0.
int operationIsGreaterThanOrEqualTo(CPU *cpu) {
    if (invalidRegisters(3, cpu->instRegister.RField,
                            cpu->instRegister.LField,
                            cpu->instRegister.MField)) { 
        //printError(ERROR_INVALID_REGISTER);
        
        return SIGNAL_FAILURE;
    }
    
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] >=
                                                cpu->registers[cpu->instRegister.MField]);
    return SIGNAL_SUCCESS;
}
