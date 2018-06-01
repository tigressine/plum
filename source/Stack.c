#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "Stack.h"
#include "VirtualMachine.h"

// Return an empty record stack.
recordStack *initializeRecordStack(void) {
    return calloc(1, sizeof(recordStack));
}

// Push a new record onto the stack.
int pushRecord(recordStack *stack,
               int parameterCount,
               int localCount,
               int returnAddress,
               int functionalValue) {
    recordStackItem *newRecord;
    
    if (stack == NULL) {
        return OP_FAILURE;
    }

    // If there is not enough memory for a new record, returns OP_FAILURE.
    if ((newRecord = malloc(sizeof(recordStackItem))) == NULL) {
        return OP_FAILURE;
    }

    // Set parameters member to NULL if there aren't any parameters.
    if (parameterCount < 1) {
        newRecord->parameters = NULL;
    }
    // Otherwise allocate an array for parameters.
    else if ((newRecord->parameters = calloc(1, sizeof(int) * parameterCount)) == NULL) {
        free(newRecord);

        return OP_FAILURE;
    }

    // Do the same process but for local variables.
    if (localCount < 1) {
        newRecord->locals = NULL;
    }
    else if ((newRecord->locals = calloc(1, sizeof(int) * localCount)) == NULL) {
        free(newRecord->parameters);
        free(newRecord);

        return OP_FAILURE;
    }

    newRecord->localCount = localCount;
    newRecord->parameterCount = parameterCount;
    newRecord->returnAddress = returnAddress;
    newRecord->functionalValue = functionalValue;
    newRecord->dynamicLink = stack->currentRecord;
    // Static links are not implemented yet.
    newRecord->staticLink = NULL;
    // Set the top of the stack to be newRecord.
    stack->currentRecord = newRecord;

    return OP_SUCCESS;
}

// Remove the top record from the stack and any associated dynamic memory.
int popRecord(recordStack *stack) {
    recordStackItem *nextItem;
    int returnValue;
    
    if (stack == NULL || stack->currentRecord == NULL) {
        return OP_FAILURE;
    }

    returnValue = stack->currentRecord->functionalValue;
    nextItem = stack->currentRecord->dynamicLink;
    free(stack->currentRecord->parameters);
    free(stack->currentRecord->locals);
    free(stack->currentRecord);
    stack->currentRecord = nextItem;

    return returnValue;
}

// NOT FINISHED
int storeValue(recordStack *stack, int depth, int index, int value) {
    int i;
    recordStackItem *desiredRecord;

    if (stack == NULL || stack->currentRecord == NULL) {
        return OP_FAILURE;
    }

    desiredRecord = stack->currentRecord;
    for (i = 0; i < depth; i++) {
        desiredRecord = desiredRecord->dynamicLink;
        if (desiredRecord == NULL) {
            return OP_FAILURE;
        }
    }

    // Because assembly lang includes some static variables. ///////
    index -= 3;
    if (index < desiredRecord->parameterCount) {
        desiredRecord->parameters[index] = value;

        return OP_SUCCESS;
    }
    else {
        index -= desiredRecord->parameterCount;
        if (index < desiredRecord->localCount) {
            desiredRecord->locals[index] = value;

            return OP_SUCCESS;
        }
        else {
            return OP_FAILURE;
        }
    }
}

// Destroy the record stack.
recordStack *destroyRecordStack(recordStack *stack) {
    // Pop all records out of the stack.
    while (popRecord(stack) != OP_FAILURE);

    // Free the stack container.
    free(stack);

    return NULL;
}
