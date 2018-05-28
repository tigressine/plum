#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define STACK_OP_FAILURE INT_MIN
#define STACK_OP_SUCCESS INT_MAX

////////////////////////////////////// RECORD STACK
typedef struct recordStackItem {
    int *parameters;
    int *locals;
    int parameterCount;
    int localCount;
    int returnAddress;
    int functionalValue;
    struct recordStackItem *dynamicLink;
    struct recordStackItem *staticLink;
} recordStackItem;

typedef struct recordStack {
    int records;
    recordStackItem *currentRecord;
} recordStack;

recordStack *initializeRecordStack(void) {
    return calloc(1, sizeof(recordStack));
}

int pushRecord(recordStack *stack,
               int parameterCount,
               int localCount,
               int returnAddress,
               int functionalValue) {
    recordStackItem *newRecord;
    
    if (stack == NULL) {
        return STACK_OP_FAILURE;
    }

    if ((newRecord = malloc(sizeof(recordStackItem))) == NULL) {
        return STACK_OP_FAILURE;
    }

    if (parameterCount < 1) {
        newRecord->parameters = NULL;
    }
    else if ((newRecord->parameters = calloc(1, sizeof(int) * parameterCount)) == NULL) {
        free(newRecord);

        return STACK_OP_FAILURE;
    }

    if (localCount < 1) {
        newRecord->locals = NULL;
    }
    else if ((newRecord->locals = calloc(1, sizeof(int) * localCount)) == NULL) {
        free(newRecord->parameters);
        free(newRecord);

        return STACK_OP_FAILURE;
    }

    newRecord->localCount = localCount;
    newRecord->parameterCount = parameterCount;
    newRecord->returnAddress = returnAddress;
    newRecord->functionalValue = functionalValue;
    newRecord->dynamicLink = stack->currentRecord;
    newRecord->staticLink = NULL; // Need function to resolve static links
    stack->currentRecord = newRecord;

    return STACK_OP_SUCCESS;
}

int popRecord(recordStack *stack) {
    recordStackItem *nextItem;
    int returnValue;
    
    if (stack == NULL || stack->currentRecord == NULL) {
        return STACK_OP_FAILURE;
    }

    returnValue = stack->currentRecord->functionalValue;
    nextItem = stack->currentRecord->dynamicLink;
    free(stack->currentRecord->parameters);
    free(stack->currentRecord->locals);
    free(stack->currentRecord);
    stack->currentRecord = nextItem;

    return returnValue;
}

int peekRecord() {


}

void printRecord(recordStackItem *record) {
    int i;
    
    if (record == NULL) {
        printf("NULL record.\n");

        return;
    }

    printf("\nRecord details:\n");
    printf("Parameters: ");
    for (i = 0; i < record->parameterCount; i++) {
        printf("%d ", record->parameters[i]);
    }
    printf("\n");

    printf("Locals: ");
    for (i = 0; i < record->localCount; i++) {
        printf("%d ", record->locals[i]);
    }
    printf("\n");

    printf("Return address: %d\n", record->returnAddress);
    printf("Functional value: %d\n", record->functionalValue);
    printf("Dynamic link: %p\n", record->dynamicLink);
    printf("Static link: %p\n", record->staticLink);
}

// NOT FINISHED
int storeValue(recordStack *stack, int depth, int index, int value) {
    int i;
    recordStackItem *desiredRecord;

    if (stack == NULL || stack->currentRecord == NULL) {
        return STACK_OP_FAILURE;
    }

    desiredRecord = stack->currentRecord;
    for (i = 0; i < depth; i++) {
        desiredRecord = desiredRecord->dynamicLink;
        if (desiredRecord == NULL) {
            return STACK_OP_FAILURE;
        }
    }

    // Because assembly lang includes some static variables. ///////
    index -= 3;
    if (index < desiredRecord->parameterCount) {
        desiredRecord->parameters[index] = value;

        return STACK_OP_SUCCESS;
    }
    else {
        index -= desiredRecord->parameterCount;
        if (index < desiredRecord->localCount) {
            desiredRecord->locals[index] = value;

            return STACK_OP_SUCCESS;
        }
        else {
            return STACK_OP_FAILURE;
        }
    }
}

recordStack *destroyRecordStack(recordStack *stack) {
    while (popRecord(stack) != STACK_OP_FAILURE);

    free(stack);

    return NULL;
}
///////////////////////////////////////////////

////////////////////////////////// DATA STACK
typedef struct dataStackItem {
    int data;
    struct dataStackItem *next;
} dataStackItem;

typedef struct dataStack {
    int size;
    dataStackItem *topDatum;
} dataStack;

// Allocate a new data stack.
dataStack *initializeDataStack(void) {
    return calloc(1, sizeof(dataStack));
}

// Push to the top of the data stack.
int pushData(dataStack *stack, int data) {
    dataStackItem *newItem;
    
    if (stack == NULL) {
        return STACK_OP_FAILURE;
    }

    if ((newItem = malloc(sizeof(dataStackItem))) == NULL) {
        return STACK_OP_FAILURE;
    }

    newItem->data = data;
    newItem->next = stack->topDatum;
    stack->topDatum = newItem;

    return STACK_OP_SUCCESS;
}

// Pop from the top of the data stack.
int popData(dataStack *stack) {
    dataStackItem *nextItem;
    int returnData;

    if (stack == NULL || stack->topDatum == NULL) {
        return STACK_OP_FAILURE;
    }

    returnData = stack->topDatum->data;
    nextItem = stack->topDatum->next;
    free(stack->topDatum);
    stack->topDatum = nextItem;

    return returnData;
}

// Peek at the top of the data stack.
int peekData(dataStack *stack) {
    if (stack == NULL || stack->topDatum == NULL) {
        return STACK_OP_FAILURE;
    }

    return stack->topDatum->data;
}

dataStack *destroyDataStack(dataStack *stack) {
    while (popData(stack) != STACK_OP_FAILURE);
    
    free(stack);

    return NULL;
}
//////////////////////////////////////////////

///////////////////////////////////////// MAIN
int main(void) {
    dataStack *data;
    recordStack *records;
    //maybe doesnt need to be a pointer here
    records = initializeRecordStack();
    data = initializeDataStack();
    
    /*
    pushData(data, 100);
    pushData(data, 200);
    pushData(data, 300);
    pushData(data, 400);
    pushData(data, 2);
    printf("%d\n", popData(data));
    printf("%d\n", popData(data));
    printf("%d\n", popData(data));
    printf("%d\n", peekData(data));
    printf("%d\n", popData(data));
    printf("%d\n", popData(NULL));
    */
    pushData(data, 300);
    pushData(data, 22);
    pushData(data, 20000);

    pushRecord(records, 2, 2, 100, 29);
    pushRecord(records, 0, 1, 99, 0);
    pushRecord(records, 3, 0, 1, 100);

    printf("%s\n", (storeValue(records, 0, 3, popData(data)) == STACK_OP_SUCCESS) ? "success" : "failure");

    storeValue(records, 1, 3, popData(data));
    storeValue(records, 2, 4, popData(data));

    printRecord(records->currentRecord);
    printRecord(records->currentRecord->dynamicLink);
    printRecord(records->currentRecord->dynamicLink->dynamicLink);

    data = destroyDataStack(data);
    records = destroyRecordStack(records);

    return 0;
}
