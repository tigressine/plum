// Part of Plum by Tiger Sachse.

#include <stdlib.h>
#include "generator.h"

// Check if queue is empty.
int isQueueEmpty(InstructionQueue *queue) {
    return (queue == NULL || queue->head == NULL);
}

// Get length of queue.
int getQueueSize(InstructionQueue *queue) {
    return (queue == NULL) ? 0 : queue->length;
}

// Create a new instruction queue.
InstructionQueue *createInstructionQueue(void) {
    InstructionQueue *new;
    
    if ((new = calloc(1, sizeof(InstructionQueue))) == NULL) {
        printError(ERROR_OUT_OF_MEMORY);
    }

    return new;
}

// Create a new instruction queue node.
QueueNode *createQueueNode(Instruction instruction) {
    QueueNode *new;

    if ((new = malloc(sizeof(QueueNode))) != NULL) {
        new->instruction = instruction;
        new->next = NULL;
    }
    else {
        printError(ERROR_OUT_OF_MEMORY);
    }

    return new;
}

// Add a new instruction to the end of the queue.
int enqueueInstruction(InstructionQueue *queue, Instruction instruction) {
    QueueNode *new;
    
    if (queue == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    } 

    // Attempt to create a new node.
    if ((new = createQueueNode(instruction)) == NULL) {
        return SIGNAL_FAILURE;
    }


    // If there are no items in the queue, then both the head and tail
    // will point to this new item.
    if (queue->tail == NULL || queue->head == NULL) {
        queue->head = new;
        queue->tail = new;
    }

    // Else add the new node to the end and adjust the tail pointer.
    else {
        queue->tail->next = new;
        queue->tail = queue->tail->next;
    }

    queue->length++;

    return SIGNAL_SUCCESS;
}

// Emit all of the instructions in the queue in order, until empty.
int emitInstructions(IOTunnel *tunnel, InstructionQueue *queue) {
    Instruction instruction;
    int returnValue;
    QueueNode *next;

    if (queue == NULL || tunnel == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }
  
    returnValue = SIGNAL_SUCCESS;

    // Emit each instruction, starting at the head of the queue. Delete
    // the nodes along the way.
    while (!isQueueEmpty(queue)) {
        instruction = queue->head->instruction;
        next = queue->head->next;
        free(queue->head);
        queue->head = next;
        queue->length--;

        // If any call to emitInstruction fails, then change the returnValue to
        // failure. Note that this doesn't just return failure immediately, else
        // the rest of the queue would never get deleted, resulting in a memory leak.
        if (emitInstruction(tunnel,
                            instruction.opCode,
                            instruction.RField,
                            instruction.LField,
                            instruction.MField) == SIGNAL_FAILURE) {
            returnValue = SIGNAL_FAILURE;
        }
    }

    free(queue);

    return returnValue;
}
