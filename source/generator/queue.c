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

// Clear all instructions out of the queue.
void clearInstructionQueue(InstructionQueue *queue) {
    QueueNode *current;
    QueueNode *next;

    if (queue == NULL) {
        return;
    }

    // Delete all nodes.
    current = queue->head;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    // Reset all values.
    queue->length = 0;
    queue->head = NULL;
    queue->tail = NULL;

    return;
}

// Delete the InstructionQueue.
void destroyInstructionQueue(InstructionQueue *queue) {
    if (queue == NULL) {
        return;
    }

    clearInstructionQueue(queue);
    free(queue);
}
