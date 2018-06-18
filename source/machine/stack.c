#include <stdlib.h>
#include "machine.h"

// Return an empty record stack.
recordStack *initializeRecordStack(void) {
    return calloc(1, sizeof(recordStack));
}

// Push a new record onto the stack.
int pushRecord(CPU *cpu, recordStack *stack) {
    recordStackItem *newRecord;
    
    if (cpu == NULL || stack == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    // If there is not enough memory for a new record, return SIGNAL_FAILURE.
    if ((newRecord = malloc(sizeof(recordStackItem))) == NULL) {
        printError(ERROR_OUT_OF_MEMORY);
        
        return SIGNAL_FAILURE;
    }

    newRecord->locals = NULL;
    newRecord->localCount = 0;
    newRecord->returnValue = 0;
    newRecord->returnAddress = cpu->programCounter;
    newRecord->dynamicLink = stack->currentRecord;
    if ((newRecord->staticLink = getStaticParent(stack, cpu->instRegister.LField)) == NULL) {
        newRecord->staticLink = newRecord; 
    }
    
    // Set the top of the stack to be newRecord.
    stack->currentRecord = newRecord;
    stack->records++;

    return SIGNAL_SUCCESS;
}

// Remove the top record from the stack and free any associated dynamic memory.
int popRecord(recordStack *stack) {
    recordStackItem *nextRecord;
    int returnValue;

    if (stack == NULL || stack->currentRecord == NULL) {
        printError(ERROR_NULL_CHECK);
        
        return SIGNAL_FAILURE;
    }

    nextRecord = stack->currentRecord->dynamicLink;
    returnValue = stack->currentRecord->returnValue;
    free(stack->currentRecord->locals);
    free(stack->currentRecord);
   
    // Set the top of the stack to the record below the old top.
    stack->currentRecord = nextRecord;
    stack->records--;

    return returnValue;
}

// Return the top record of the stack.
recordStackItem *peekRecord(recordStack *stack) {
    return (stack == NULL) ? NULL : stack->currentRecord;
}

// Allocate the locals array in the given record.
int allocateLocals(recordStackItem *record, int localCount) {
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
recordStackItem *getDynamicParent(recordStack *stack, int levels) {
    recordStackItem *desiredRecord;

    if (stack == NULL) {
        printError(ERROR_NULL_CHECK);
        
        return NULL;
    }
    
    desiredRecord = stack->currentRecord;
  
    // While there are still more levels to go and desiredRecord hasn't reached
    // the bottom of the stack, go deeper.
    while (levels > 0 && desiredRecord != NULL) {
        desiredRecord = desiredRecord->dynamicLink;
        levels--;
    }

    return desiredRecord;
}

// Seek down the stack through static links (in lexicographical order).
recordStackItem *getStaticParent(recordStack *stack, int levels) {
    recordStackItem *desiredRecord;

    if (stack == NULL) {
        printError(ERROR_NULL_CHECK);
        
        return NULL;
    }

    desiredRecord = stack->currentRecord;
    
    // While there are still more levels to go and desiredRecord hasn't reached
    // the bottom of the stack, go deeper.
    while (levels > 0 && desiredRecord != NULL) {
        desiredRecord = desiredRecord->staticLink;
        levels--;
    }

    return desiredRecord;
}

// Return if stack is empty or not.
int isEmpty(recordStack *stack) {
    return (stack == NULL || stack->records == 0);
}

// Destroy the record stack.
recordStack *destroyRecordStack(recordStack *stack) {
    // Pop all records out of the stack.
    while (!isEmpty(stack)) {
        popRecord(stack);
    }

    // Free the stack container.
    free(stack);

    return NULL;
}
