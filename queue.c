#include "queue.h"

/* createQueue
 * input: none
 * output: a pointer to a queue (this is malloc-ed so must be freed eventually!)
 *
 * Creates a new empty queue and returns a pointer to it.
 */
Queue *createQueue() {
    Queue *pq = (Queue *) malloc(sizeof(Queue));
    if(pq != NULL) {
        pq->qFront = NULL;
        pq->qRear = NULL;
    }

    return pq;
}

/* freeStack
 * input: a pointer to a Queue
 * output: none
 *
 * frees the given Queue and all its contents
 */
void freeQueue(Queue *pq) {
    while(!isEmpty(pq)) {
        free(dequeue(pq));
    }
    free(pq);
}

/* getNext
 * input: a pointer to a Queue
 * output: a pointer to process
 *
 * Returns the process stored at the front of the Queue.  It does not remove the element from the queue.
 */
queueType getNext(Queue *pq) {
    if(isEmpty(pq)) {
        /* no element to return */
        return NULL;
    }
    return pq->qFront->qt;
}

/* dequeue
 * input: a pointer to a Queue
 * output: a pointer to process
 *
 * Dequeues and returns the process stored at the front of the Queue.  It does not free the dequeue-ed element.
 */
queueType dequeue( Queue *pq ) {
    LLNode *temp;
    queueType qt;

    if(isEmpty(pq)) {
        /* no element to return */
        printf("error: queue empty\n");
        return NULL;
    }
    temp = pq->qFront;
    pq->qFront = pq->qFront->pNext;
    if(pq->qFront == NULL) {
        pq->qRear = NULL;
    }
    qt = temp->qt;
    free(temp);

    return qt;
}

/* enqueue
 * input: a pointer to a Queue, a queueType
 * output: none
 *
 * Inserts the process on the rear of the given Queue.
 */
void enqueue(Queue *pq, queueType qt) {
    LLNode *node = (LLNode*) malloc(sizeof(LLNode));
    if(node == NULL) {
        fprintf(stderr, "enqueue: Failed to allocate memory");
        exit(-1);
    }

    node->pNext = NULL;
    node->qt = qt;
    if(isEmpty(pq)) {
        pq->qFront = node;
        pq->qRear = node;
        return;
    }
    pq->qRear->pNext = node;
    pq->qRear = node;
}

/* isEmpty
 * input: a pointer to a Queue
 * output: a boolean
 *
 * returns TRUE if the Queue is empty and FALSE otherwise
 */
bool isEmpty(Queue *pq) {
    if(pq->qFront == NULL && pq->qRear == NULL) {
        return true;
    } else if(pq->qFront == NULL || pq->qRear == NULL) {
        fprintf(stderr, "isEmpty: Queue had inconsistent values for front and rear.\n");
        exit(-1);
    }
    return false;
}