// Part of Plum by Tiger Sachse.

#include <stdlib.h>
#include "generator.h"

// NEED ERRORS

typedef struct QueueNode {
    Instruction instruction;
    struct QueueNode *next;
} QueueNode;

typedef struct InstructionQueue {
    QueueNode *head;
    QueueNode *tail;
    int length;
} InstructionQueue;

int isQueueEmpty(InstructionQueue *queue) {
    return (queue == NULL || queue->head == NULL);
}

int getSize(InstructionQueue *queue) {
    return (queue == NULL) ? 0 : queue->length;
}

InstructionQueue *createInstructionQueue(void) {
    return calloc(1, sizeof(InstructionQueue));
}

QueueNode *createQueueNode(Instruction instruction) {
    QueueNode *new;

    if ((new = malloc(sizeof(QueueNode))) != NULL) {
        new->instruction = instruction;
        new->next = NULL;
    }

    return new;
}

int enqueueInstruction(InstructionQueue *queue, Instruction instruction) {
    QueueNode *new;
    
    if (queue == NULL) {
        return SIGNAL_FAILURE;
    } 

    if ((new = createQueueNode(instruction)) == NULL) {
        return SIGNAL_FAILURE;
    }


    if (queue->tail == NULL || queue->head == NULL) {
        queue->head = new;
        queue->tail = new;
    }
    else {
        queue->tail->next = new;
    }
    queue->length++;

    return SIGNAL_SUCCESS;
}


int emitInstructions(InstructionQueue *queue, IOTunnel *tunnel) {
    Instruction instruction;

    if (queue == NULL || tunnel == NULL) {
        return SIGNAL_FAILURE;
    }
    
    while (!isQueueEmpty(queue)) {
        instruction = queue->head->instruction;
        queue->head = queue->head->next;
        queue->length--;

        if (emitInstruction(tunnel,
                        instruction.opCode,
                        instruction.RField,
                        instruction.LField,
                        instruction.MField) == SIGNAL_FAILURE) {
            return SIGNAL_FAILURE;
        }
    }
}
