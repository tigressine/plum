#include <stdlib.h>
#include "VirtualMachine.h"

// Return an empty record stack.
recordStack *initializeRecordStack(void) {
    return calloc(1, sizeof(recordStack));
}

// Push a new record onto the stack.
int pushRecord(CPU *cpu, recordStack *stack) {
    recordStackItem *newRecord;
    
    if (cpu == NULL || stack == NULL) {
        return OP_FAILURE;
    }

    // If there is not enough memory for a new record, returns OP_FAILURE.
    if ((newRecord = malloc(sizeof(recordStackItem))) == NULL) {
        return OP_FAILURE;
    }

    newRecord->locals = NULL;
    newRecord->localCount = 0;
    newRecord->returnValue = 0;
    newRecord->dynamicLinkValue = 0;//still unknown //
    newRecord->staticLinkValue = 0;//sstill unkown //
    newRecord->returnAddress = cpu->programCounter;
    newRecord->dynamicLink = stack->currentRecord;
    newRecord->staticLink = getStaticParent(stack, cpu->instRegister.LField);//
    
    // Set the top of the stack to be newRecord.
    stack->currentRecord = newRecord;
    stack->records++;
    cpu->stackPointer += 4;//

    return OP_SUCCESS;
}

// Remove the top record from the stack and any associated dynamic memory.
int popRecord(recordStack *stack) {
    recordStackItem *nextRecord;
    int returnValue;

    if (stack == NULL || stack->currentRecord == NULL) {
        return OP_FAILURE;
    }

    nextRecord = stack->currentRecord->dynamicLink;
    returnValue = stack->currentRecord->returnValue;
    free(stack->currentRecord->locals);
    free(stack->currentRecord);
    
    stack->currentRecord = nextRecord;
    stack->records--;

    return returnValue;
}

recordStackItem *peekRecord(recordStack *stack) {
    return (stack == NULL) ? NULL : stack->currentRecord;
}

int allocateLocals(recordStackItem *record, int localCount) {
    if (record == NULL || record->locals != NULL) {
        return OP_FAILURE;
    }

    record->localCount = localCount;
    
    // Set locals to NULL if there aren't any parameters.
    if (localCount < 1) {
        record->locals = NULL;
    }
    // Else make a new locals array in the record.
    else if ((record->locals = calloc(1, sizeof(int) * localCount)) == NULL) {
        record->localCount = 0;
        
        return OP_FAILURE;
    }
    
    return OP_SUCCESS;
}

recordStackItem *getDynamicParent(recordStack *stack, int levels) {
    recordStackItem *desiredRecord;

    if (stack == NULL) {
        return NULL;
    }
    
    desiredRecord = stack->currentRecord;
   
    while (levels > 0 && desiredRecord != NULL) {
        desiredRecord = desiredRecord->dynamicLink;
        levels--;
    }

    return desiredRecord;
}

recordStackItem *getStaticParent(recordStack *stack, int levels) {
    recordStackItem *desiredRecord;

    if (stack == NULL) {
        return NULL;
    }

    desiredRecord = stack->currentRecord;
    
    while (levels > 0 && desiredRecord != NULL) {
        desiredRecord = desiredRecord->staticLink;
        levels--;
    }

    return desiredRecord;
}

int isEmpty(recordStack *stack) {
    return (stack == NULL || stack->records == 0) ? 1 : 0;
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
