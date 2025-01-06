#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int *data;
    int front;
    int rear;
    int capacity;
    int size;
} queue;

queue* createQueue(int capacity) {
    queue *q = (queue*)malloc(sizeof(queue));
    q->data = (int*)malloc(capacity * sizeof(int));
    q->front = 0;
    q->rear = -1;
    q->capacity = capacity;
    q->size = 0;
    return q;
}

void enqueue(queue *q, int value) {
    if (isFull(q)) {
        int newCapacity = q->capacity * 2;
        int *newData = (int *)realloc(q->data, newCapacity * sizeof(int));
        if (!newData) {
            printf("Memory allocation failed while resizing the queue!\n");
            return;
        }
        if (q->front > q->rear) {
            for (int i = 0; i < q->front; i++) {
                newData[i + q->capacity] = q->data[i];
            }
            q->rear += q->capacity;
        }

        q->data = newData;
        q->capacity = newCapacity;
        // printf("Queue capacity increased to %d\n", q->capacity);
    }

    q->rear = (q->rear + 1) % q->capacity;
    q->data[q->rear] = value;
    q->size++;
}

int dequeue(queue *q) {
    if (isEmpty(q)) {
        printf("Queue is empty!\n");
        return -1;
    }
    int value = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;
    return value;
}

int isEmpty(queue *q) {
    return q->size == 0;
}

int isFull(queue *q) {
    return q->size == q->capacity;
}

void clearQueue(queue *q) {
    if (!q) {
        // printf("Queue is not initialized!\n");
        return;
    }

    q->front = 0;
    q->rear = -1;
    q->size = 0;

    // printf("Queue cleared successfully.\n");
}



// for (int i = 0; i < MAX_DIRECTORY_ENTRIES; i++) {
    //     snprintf(dir.entries[i].name, sizeof(dir.entries[i].name), "Entry %d", i);
    //     dir.entries[i].size = 0;
    //     dir.entries[i].starting_block = -1; // Invalid block
    //     dir.entries[i].parent = -1; // No parent
    //     dir.entries[i].isDelete = false;
    //     dir.entries[i].isFree = true;
    // }

    // for (int i = 0; i < MAX_BLOCKS; i++) {
    //     fat.entries[i].isFree = true;
    //     fat.entries[i].next = -1; // No next block
    // }

    // saveDirectory(&dir);
    // saveFAT(&fat);