// Part of Plum by Tiger Sachse.

#include <stdlib.h>
#include "machine.h"

// Return an empty record stack.
RecordStack *initializeRecordStack(void) {
    return calloc(1, sizeof(RecordStack));
}

// Push a new record onto the stack.
int pushRecord(CPU *cpu, RecordStack *stack) {
    RecordStackItem *new;
    
    if (cpu == NULL || stack == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    // If there is not enough memory for a new record, return SIGNAL_FAILURE.
    if ((new = malloc(sizeof(RecordStackItem))) == NULL) {
        printError(ERROR_OUT_OF_MEMORY);
        
        return SIGNAL_FAILURE;
    }

    new->locals = NULL;
    new->localCount = 0;
    new->returnValue = 0;
    new->returnAddress = cpu->programCounter;
    new->dynamicLink = stack->currentRecord;
    if ((new->staticLink = getStaticParent(stack, cpu->instRegister.LField)) == NULL) {
        new->staticLink = new; 
    }
    
    // Set the top of the stack to be new.
    stack->currentRecord = new;
    stack->records++;

    return SIGNAL_SUCCESS;
}

// Remove the top record from the stack and free any associated dynamic memory.
int popRecord(RecordStack *stack) {
    RecordStackItem *next;
    int returnValue;

    if (stack == NULL || stack->currentRecord == NULL) {
        printError(ERROR_NULL_CHECK);
        
        return SIGNAL_FAILURE;
    }

    next = stack->currentRecord->dynamicLink;
    returnValue = stack->currentRecord->returnValue;
    free(stack->currentRecord->locals);
    free(stack->currentRecord);
   
    // Set the top of the stack to the record below the old top.
    stack->currentRecord = next;
    stack->records--;

    return returnValue;
}

// Return the top record of the stack.
RecordStackItem *peekRecord(RecordStack *stack) {
    return (stack == NULL) ? NULL : stack->currentRecord;
}

// Allocate the locals array in the given record.
int allocateLocals(RecordStackItem *record, int localCount) {
    if (record == NULL || record->locals != NULL) {
        printError(ERROR_NULL_CHECK);
        
        return SIGNAL_FAILURE;
    }

    record->localCount = localCount;
    
    // Set locals to NULL if there aren't any parameters.
    if (localCount < 1) {
        record->locals = NULL;
    }
    // Else make a new locals array in the record.
    else if ((record->locals = calloc(1, sizeof(int) * localCount)) == NULL) {
        record->localCount = 0;
        printError(ERROR_OUT_OF_MEMORY);

        return SIGNAL_FAILURE;
    }
    
    return SIGNAL_SUCCESS;
}

// Seek down the stack through dynamic links (in order).
RecordStackItem *getDynamicParent(RecordStack *stack, int levels) {
    RecordStackItem *desired;

    if (stack == NULL) {
        printError(ERROR_NULL_CHECK);
        
        return NULL;
    }
    
    desired = stack->currentRecord;
  
    // While there are still more levels to go and desired hasn't reached
    // the bottom of the stack, go deeper.
    while (levels > 0 && desired != NULL) {
        desired = desired->dynamicLink;
        levels--;
    }

    if (desired == NULL) {
        printError(ERROR_INVALID_DYNAMIC_PARENT);
    }

    return desired;
}

// Seek down the stack through static links (in lexicographical order).
RecordStackItem *getStaticParent(RecordStack *stack, int levels) {
    RecordStackItem *desired;

    if (stack == NULL) {
        printError(ERROR_NULL_CHECK);
        
        return NULL;
    }

    desired = stack->currentRecord;
    
    // While there are still more levels to go and desired hasn't reached
    // the bottom of the stack, go deeper.
    while (levels > 0 && desired != NULL) {
        desired = desired->staticLink;
        levels--;
    }

    if (desired == NULL) {
        printError(ERROR_INVALID_STATIC_PARENT);
    }

    return desired;
}

// Return if stack is empty or not.
int isEmpty(RecordStack *stack) {
    return (stack == NULL || stack->records == 0);
}

// Destroy the record stack.
RecordStack *destroyRecordStack(RecordStack *stack) {
    // Pop all records out of the stack.
    while (!isEmpty(stack)) {
        popRecord(stack);
    }

    // Free the stack container.
    free(stack);

    return NULL;
}
