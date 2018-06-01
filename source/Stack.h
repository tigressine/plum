#include <limits.h>

#ifndef STACK_HEADER
#define STACK_HEADER

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

recordStack *initializeRecordStack(void);
int pushRecord(recordStack*, int, int, int, int);
int popRecord(recordStack*);
int storeValue(recordStack*, int, int, int);
recordStack *destroyRecordStack(recordStack*);
#endif
