#include <limits.h>

#ifndef STACK_HEADER
#define STACK_HEADER

// An item in an activation record stack. Also serves as a node in
// a linked list (that's how the stack is implemented). Many of these
// members are unneccesary except for the project specifications.
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

// Container struct for a record linked list struct. Also keeps track
// of the number of nodes in the stack.
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
